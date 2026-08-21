#pragma once

#include "../DrawTypes.h"
#include <algorithm>
#include <string>
#include <format>
#include <stdarg.h>

#include <glm/vec4.hpp>

#ifndef GUIZMO_NAMESPACE
#define GUIZMO_NAMESPACE Guizmo
#endif
#include <glm/ext/matrix_float4x4.hpp>

class IRenderPlatform;
class IRenderDevice;
struct DrawList;
struct GuiWindow;
struct RenderContext;

inline U32 ColorConvertFloat4ToU32(const Float4& in)
{
    U32 r = static_cast<U32>(std::clamp(in.r, 0.0f, 1.0f) * 255.0f + 0.5f);
    U32 g = static_cast<U32>(std::clamp(in.g, 0.0f, 1.0f) * 255.0f + 0.5f);
    U32 b = static_cast<U32>(std::clamp(in.b, 0.0f, 1.0f) * 255.0f + 0.5f);
    U32 a = static_cast<U32>(std::clamp(in.a, 0.0f, 1.0f) * 255.0f + 0.5f);

    return (a << 24) | (b << 16) | (g << 8) | r;
}

inline WidgetID HashStr(const char* str, size_t length, WidgetID seed = 2166136261U)
{
    WidgetID hash = seed;

    if (length == 0 && str != nullptr) {
        length = std::strlen(str);
    }

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<unsigned char>(str[i]);
        hash *= 16777619U;
    }

    return hash;
}

inline WidgetID HashData(const void* data, size_t data_size, WidgetID seed = 2166136261U)
{
    WidgetID hash = seed;
    const unsigned char* bytes = static_cast<const unsigned char*>(data);

    for (size_t i = 0; i < data_size; ++i)
    {
        hash ^= bytes[i];
        hash *= 16777619U;
    }

    return hash;
}

inline std::string FormatString(const char* format, ...)
{
    va_list args;

    va_start(args, format);
    int size = vsnprintf(nullptr, 0, format, args);
    va_end(args);

    if (size <= 0) return "";

    std::string buffer;
    buffer.resize(size);

    va_start(args, format);
    vsnprintf(&buffer[0], size + 1, format, args);
    va_end(args);

    return buffer;
}

namespace GUIZMO_NAMESPACE
{
   bool IsOver();

   // return true if mouse IsOver or if the gizmo is in moving state
   bool IsUsing();

   // return true if the view gizmo is in moving state
   bool IsUsingViewManipulate();
   // only check if your mouse is over the view manipulator - no matter whether it's active or not
   bool IsViewManipulateHovered();

   // return true if any gizmo is in moving state
   bool IsUsingAny();

   // enable/disable the gizmo. Stay in the state until next call to Enable.
   // gizmo is rendered with gray half transparent color when disabled
   void Enable(bool enable);

   // helper functions for manualy editing translation/rotation/scale with an input float
   // translation, rotation and scale float points to 3 floats each
   // Angles are in degrees (more suitable for human editing)
   // example:
   // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
   // Guizmo::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
   // Gui::InputFloat3("Tr", matrixTranslation, 3);
   // Gui::InputFloat3("Rt", matrixRotation, 3);
   // Gui::InputFloat3("Sc", matrixScale, 3);
   // Guizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);
   //
   // These functions have some numerical stability issues for now. Use with caution.
   void DecomposeMatrixToComponents(const float* matrix, float* translation, float* rotation, float* scale);
   void RecomposeMatrixFromComponents(const float* translation, const float* rotation, const float* scale, float* matrix);
   glm::vec4 BuildPlan(const glm::vec4& point, const glm::vec4& normal);

   void SetRect(float x, float y, float width, float height);
   // default is false
   void SetOrthographic(bool isOrthographic);

   // Render coordinate system axes (red X, green Y and blue Z). Usefull for debug/tests
   void DrawAxes(const float* view, const float* projection, const float* matrices, int matrixCount);
   // Render a cube with face color corresponding to face normal. Usefull for debug/tests
   void DrawCubes(const float* view, const float* projection, const float* matrices, int matrixCount);
   void DrawGrid(const float* view, const float* projection, const float* matrix, const float gridSize);
   // Render grid with customizable major line step and amount of segments between major lines.
   // NOTE(m.wlasiuk) : calling this function with majorStep = 1.0f and subdivision = 1 is equivalent to DrawGrid in terms of the end result but performs more calculations
   void DrawGridCustom(const float* view, const float* projection, const float* matrix, const float gridSize, const float majorStep, const unsigned int subdivision);
   // Render grid with customizable major line step and amount of segments between major lines and with possibility to set custom colors for major, minor and center lines
   void DrawGridCustomColor(const float* view, const float* projection, const float* matrix, const float gridSize, const float majorStep, const unsigned int subdivision, const U32 majorCol, const U32 minorCol, const U32 centerCol);

   // call it when you want a gizmo
   // Needs view and projection matrices.
   // matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional
   // translation is applied in world space
   enum OPERATION
   {
      TRANSLATE_X      = (1u << 0),
      TRANSLATE_Y      = (1u << 1),
      TRANSLATE_Z      = (1u << 2),
      ROTATE_X         = (1u << 3),
      ROTATE_Y         = (1u << 4),
      ROTATE_Z         = (1u << 5),
      ROTATE_SCREEN    = (1u << 6),
      SCALE_X          = (1u << 7),
      SCALE_Y          = (1u << 8),
      SCALE_Z          = (1u << 9),
      BOUNDS           = (1u << 10),
      SCALE_XU         = (1u << 11),
      SCALE_YU         = (1u << 12),
      SCALE_ZU         = (1u << 13),

      TRANSLATE = TRANSLATE_X | TRANSLATE_Y | TRANSLATE_Z,
      ROTATE = ROTATE_X | ROTATE_Y | ROTATE_Z | ROTATE_SCREEN,
      SCALE = SCALE_X | SCALE_Y | SCALE_Z,
      SCALEU = SCALE_XU | SCALE_YU | SCALE_ZU, // universal
      UNIVERSAL = TRANSLATE | ROTATE | SCALEU
   };

   inline OPERATION operator|(OPERATION lhs, OPERATION rhs)
   {
     return static_cast<OPERATION>(static_cast<int>(lhs) | static_cast<int>(rhs));
   }

   inline OPERATION operator&(OPERATION lhs, OPERATION rhs)
   {
       return static_cast<OPERATION>(static_cast<int>(lhs) & static_cast<int>(rhs));
   }

   inline bool operator!=(OPERATION lhs, int rhs)
   {
       return static_cast<int>(lhs) != rhs;
   }

   inline bool Intersects(OPERATION lhs, OPERATION rhs)
   {
       return (lhs & rhs) != 0;
   }

   // True if lhs contains rhs
   inline bool Contains(OPERATION lhs, OPERATION rhs)
   {
       return (lhs & rhs) == rhs;
   }

   enum MODE
   {
      LOCAL,
      WORLD
   };
   //static void ComputeCameraRay(glm::vec4& rayOrigin, glm::vec4& rayDir, glm::vec2 position, glm::vec2 size);
   Ray ComputeCameraRay();
   bool Manipulate(const float* view, const float* projection, OPERATION operation, MODE mode, float* matrix, float* deltaMatrix = NULL, const float* snap = NULL, const float* localBounds = NULL, const float* boundsSnap = NULL);
   //
   // Please note that this cubeview is patented by Autodesk : https://patents.google.com/patent/US7782319B2/en
   // It seems to be a defensive patent in the US. I don't think it will bring troubles using it as
   // other software are using the same mechanics. But just in case, you are now warned!
   //
   void ViewManipulate(float* view, float length, glm::vec2 position, glm::vec2 size, U32 backgroundColor);

   // use this version if you did not call Manipulate before and you are just using ViewManipulate
   void ViewManipulate(float* view, const float* projection, OPERATION operation, MODE mode, float* matrix, float length, glm::vec2 position, glm::vec2 size, U32 backgroundColor);

   void SetAlternativeWindow(GuiWindow* window);

   [[deprecated("Use PushID/PopID instead.")]]
   void SetID(int id);

	// ID stack/scopes
	// Read the FAQ (docs/FAQ.md or http://deargui.org/faq) for more details about how ID are handled in dear gui.
	// - Those questions are answered and pacted by understanding of the ID stack system:
	//   - "Q: Why is my widget not reacting when I click on it?"
	//   - "Q: How can I have widgets with an empty label?"
	//   - "Q: How can I have multiple widgets with the same label?"
	// - Short version: ID are hashes of the entire ID stack. If you are creating widgets in a loop you most likely
	//   want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
	// - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
	// - In this header file we use the "label"/"name" terminology to denote a string that will be displayed + used as an ID,
	//   whereas "str_id" denote a string that is only used as an ID and not normally displayed.
	void          PushID(const char* str_id);                                     // push string into the ID stack (will hash string).
	void          PushID(const char* str_id_begin, const char* str_id_end);       // push string into the ID stack (will hash string).
	void          PushID(const void* ptr_id);                                     // push pointer into the ID stack (will hash pointer).
	void          PushID(int int_id);                                             // push integer into the ID stack (will hash integer).
	void          PopID();                                                        // pop from the ID stack.
	WidgetID       GetID(const char* str_id);                                      // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into GuiStorage yourself
    WidgetID       GetID(const char* str_id_begin, const char* str_id_end);
    WidgetID       GetID(const void* ptr_id);

   // return true if the cursor is over the operation's gizmo
   bool IsOver(OPERATION op);
   void SetGizmoSizeClipSpace(float value);
   // Handle type used by the translate/rotate/scale gizmos.
   enum MOVETYPE
   {
      MT_NONE,
      MT_MOVE_X,
      MT_MOVE_Y,
      MT_MOVE_Z,
      MT_MOVE_YZ,
      MT_MOVE_ZX,
      MT_MOVE_XY,
      MT_MOVE_SCREEN,
      MT_ROTATE_X,
      MT_ROTATE_Y,
      MT_ROTATE_Z,
      MT_ROTATE_SCREEN,
      MT_SCALE_X,
      MT_SCALE_Y,
      MT_SCALE_Z,
      MT_SCALE_XYZ
   };

   // Returns which handle is actively being dragged, or MT_NONE.
   MOVETYPE GetActiveHandleType();
   // Returns which handle is currently hovered, or MT_NONE.
   MOVETYPE GetHoveredHandleType();
   // Aliases matching the MOVETYPE enum name.
   MOVETYPE GetActiveMoveType();
   MOVETYPE GetHoveredMoveType();

   // Allow axis to flip
   // When true (default), the guizmo axis flip for better visibility
   // When false, they always stay along the positive world/local axis
   void AllowAxisFlip(bool value);

   // Configure the lit where axis are hidden
   void SetAxisLit(float value);
   // Set an axis mask to permanently hide a given axis (true -> hidden, false -> shown)
   void SetAxisMask(bool x, bool y, bool z);
   // Configure the lit where planes are hiden
   void SetPlaneLit(float value);
   // from a x,y,z point in space and using Manipulation view/projection matrix, check if mouse is in pixel radius distance of that projected point
   bool IsOver(float* position, float pixelRadius);

   enum COLOR
   {
      DIRECTION_X,      // directionColor[0]
      DIRECTION_Y,      // directionColor[1]
      DIRECTION_Z,      // directionColor[2]
      PLANE_X,          // planeColor[0]
      PLANE_Y,          // planeColor[1]
      PLANE_Z,          // planeColor[2]
      SELECTION,        // selectionColor
      INACTIVE,         // inactiveColor
      TRANSLATION_LINE, // translationLineColor
      SCALE_LINE,
      ROTATION_USING_BORDER,
      ROTATION_USING_FILL,
      HATCHED_AXIS_LINES,
      TEXT,
      TEXT_SHADOW,
      COUNT
   };

   void DrawTransformAxes(DrawList* drawList, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& transform);

   void UpdateContext(const RenderContext& ctx, DrawList& list, IRenderPlatform* platform, IRenderDevice* device);

   struct Style
   {
      Style();

      float TranslationLineThickness;   // Thickness of lines for translation gizmo
      float TranslationLineArrowSize;   // Size of arrow at the end of lines for translation gizmo
      float RotationLineThickness;      // Thickness of lines for rotation gizmo
      float RotationOuterLineThickness; // Thickness of line surrounding the rotation gizmo
      float ScaleLineThickness;         // Thickness of lines for scale gizmo
      float ScaleLineCircleSize;        // Size of circle at the end of lines for scale gizmo
      float HatchedAxisLineThickness;   // Thickness of hatched axis lines
      float CenterCircleSize;           // Size of circle at the center of the translate/scale gizmo

      Float4 Colors[COLOR::COUNT];
   };

   Style& GetStyle();
}
