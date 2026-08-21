#include "ImGuizmo.h"

#include <vector>
#include <core/platform/input/IInput.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "../DrawList.hpp"
#include <core/platform/render/IRenderPlatform.hpp>
#include <render/RenderContext.hpp>
#include <ecs/Scene.hpp>
#include <ecs/components/Transform.hpp>
#include <math/CameraMath.h>
#include <glm/gtc/type_ptr.hpp>

namespace GUIZMO_NAMESPACE
{
  static const float ZPI = 3.14159265358979323846f;
  static const float RAD2DEG = (180.f / ZPI);
  static const float DEG2RAD = (ZPI / 180.f);
  const float screenRotateSize = 0.06f;
  // scale a bit so translate axis do not touch when in universal
  const float rotationDisplayFactor = 1.2f;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // utility and math

  void FPU_MatrixF_x_MatrixF(const float* a, const float* b, float* r)
  {
     r[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
     r[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
     r[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
     r[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

     r[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
     r[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
     r[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
     r[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

     r[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
     r[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
     r[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
     r[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

     r[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
     r[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
     r[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
     r[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
  }

  void Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16, bool rightHanded = true)
  {
     float temp, temp2, temp3, temp4;
     temp = 2.0f * znear;
     temp2 = right - left;
     temp3 = top - bottom;
     temp4 = zfar - znear;
     float sign = rightHanded ? -1.0f : 1.0f;
     m16[0] = temp / temp2;
     m16[1] = 0.0;
     m16[2] = 0.0;
     m16[3] = 0.0;
     m16[4] = 0.0;
     m16[5] = temp / temp3;
     m16[6] = 0.0;
     m16[7] = 0.0;
     m16[8] = (right + left) / temp2;
     m16[9] = (top + bottom) / temp3;
     m16[10] = sign * (zfar + znear) / temp4;
     m16[11] = sign;
     m16[12] = 0.0;
     m16[13] = 0.0;
     m16[14] = (-temp * zfar) / temp4;
     m16[15] = 0.0;
  }

  void Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16, bool rightHanded = true)
  {
     float ymax, xmax;
     ymax = znear * tanf(fovyInDegrees * DEG2RAD);
     xmax = ymax * aspectRatio;
     Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16, rightHanded);
  }

  void Cross(const float* a, const float* b, float* r)
  {
     r[0] = a[1] * b[2] - a[2] * b[1];
     r[1] = a[2] * b[0] - a[0] * b[2];
     r[2] = a[0] * b[1] - a[1] * b[0];
  }

  float Dot(const float* a, const float* b)
  {
     return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
  }

  void Normalize(const float* a, float* r)
  {
     float il = 1.f / (sqrtf(Dot(a, a)) + FLT_EPSILON);
     r[0] = a[0] * il;
     r[1] = a[1] * il;
     r[2] = a[2] * il;
  }

  void LookAt(const float* eye, const float* at, const float* up, float* m16, bool rightHanded = true)
  {
     float X[3], Y[3], Z[3], tmp[3];

     if (rightHanded)
     {
        tmp[0] = eye[0] - at[0];
        tmp[1] = eye[1] - at[1];
        tmp[2] = eye[2] - at[2];
     }
     else
     {
        tmp[0] = at[0] - eye[0];
        tmp[1] = at[1] - eye[1];
        tmp[2] = at[2] - eye[2];
     }
     Normalize(tmp, Z);
     Normalize(up, Y);
     Cross(Y, Z, tmp);
     Normalize(tmp, X);
     Cross(Z, X, tmp);
     Normalize(tmp, Y);

     m16[0] = X[0];
     m16[1] = Y[0];
     m16[2] = Z[0];
     m16[3] = 0.0f;
     m16[4] = X[1];
     m16[5] = Y[1];
     m16[6] = Z[1];
     m16[7] = 0.0f;
     m16[8] = X[2];
     m16[9] = Y[2];
     m16[10] = Z[2];
     m16[11] = 0.0f;
     m16[12] = -Dot(X, eye);
     m16[13] = -Dot(Y, eye);
     m16[14] = -Dot(Z, eye);
     m16[15] = 1.0f;
  }

  template <typename T> T Clamp(T x, T y, T z) { return ((x < y) ? y : ((x > z) ? z : x)); }
  template <typename T> T max(T x, T y) { return (x > y) ? x : y; }
  template <typename T> T min(T x, T y) { return (x < y) ? x : y; }
  template <typename T> bool IsWithin(T x, T y, T z) { return (x >= y) && (x <= z); }

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //

  static bool IsTranslateType(MOVETYPE type)
  {
    return type >= MT_MOVE_X && type <= MT_MOVE_SCREEN;
  }

  static bool IsRotateType(MOVETYPE type)
  {
    return type >= MT_ROTATE_X && type <= MT_ROTATE_SCREEN;
  }

  static bool IsScaleType(MOVETYPE type)
  {
    return type >= MT_SCALE_X && type <= MT_SCALE_XYZ;
  }

  // Matches MT_MOVE_AB order
  static const OPERATION TRANSLATE_PLANS[3] = { TRANSLATE_Y | TRANSLATE_Z, TRANSLATE_X | TRANSLATE_Z, TRANSLATE_X | TRANSLATE_Y };

  Style::Style()
  {
     // default values
     TranslationLineThickness   = 3.0f;
     TranslationLineArrowSize   = 6.0f;
     RotationLineThickness      = 2.0f;
     RotationOuterLineThickness = 3.0f;
     ScaleLineThickness         = 3.0f;
     ScaleLineCircleSize        = 6.0f;
     HatchedAxisLineThickness   = 6.0f;
     CenterCircleSize           = 6.0f;

     // initialize default colors
     Colors[DIRECTION_X]           = Float4(0.666f, 0.000f, 0.000f, 1.000f);
     Colors[DIRECTION_Y]           = Float4(0.000f, 0.666f, 0.000f, 1.000f);
     Colors[DIRECTION_Z]           = Float4(0.000f, 0.000f, 0.666f, 1.000f);
     Colors[PLANE_X]               = Float4(0.666f, 0.000f, 0.000f, 0.380f);
     Colors[PLANE_Y]               = Float4(0.000f, 0.666f, 0.000f, 0.380f);
     Colors[PLANE_Z]               = Float4(0.000f, 0.000f, 0.666f, 0.380f);
     Colors[SELECTION]             = Float4(1.000f, 0.500f, 0.062f, 0.541f);
     Colors[INACTIVE]              = Float4(0.600f, 0.600f, 0.600f, 0.600f);
     Colors[TRANSLATION_LINE]      = Float4(0.666f, 0.666f, 0.666f, 0.666f);
     Colors[SCALE_LINE]            = Float4(0.250f, 0.250f, 0.250f, 1.000f);
     Colors[ROTATION_USING_BORDER] = Float4(1.000f, 0.500f, 0.062f, 1.000f);
     Colors[ROTATION_USING_FILL]   = Float4(1.000f, 0.500f, 0.062f, 0.500f);
     Colors[HATCHED_AXIS_LINES]    = Float4(0.000f, 0.000f, 0.000f, 0.500f);
     Colors[TEXT]                  = Float4(1.000f, 1.000f, 1.000f, 1.000f);
     Colors[TEXT_SHADOW]           = Float4(0.000f, 0.000f, 0.000f, 1.000f);
  }

  struct Context
  {
     Context() : mbUsing(false), mbUsingViewManipulate(false), mbEnable(true), mIsViewManipulatorHovered(false), mbUsingBounds(false)
     {
     }

     DrawList* mDrawList;
     IInput* mInput = nullptr;
     IWindow* mWindow = nullptr;
     IRenderPlatform* mPlatform = nullptr;
     Style mStyle;

     MODE mMode;
     glm::mat4 mViewMat;
     glm::mat4 mProjectionMat;
     glm::mat4 mModel;
     glm::mat4 mModelLocal; // orthonormalized model
     glm::mat4 mModelInverse;
     glm::mat4 mModelSource;
     glm::mat4 mModelSourceInverse;
     glm::mat4 mMVP;
     glm::mat4 mMVPLocal; // MVP with full model matrix whereas mMVP's model matrix might only be translation in case of World space edition
     glm::mat4 mViewProjection;

     glm::vec4 mModelScaleOrigin;
     glm::vec4 mCameraEye;
     glm::vec4 mCameraRight;
     glm::vec4 mCameraDir;
     glm::vec4 mCameraUp;
     glm::vec4 mRayOrigin;
     glm::vec4 mRayVector;

     float  mRadiusSquareCenter;
     glm::vec2 mScreenSquareCenter;
     glm::vec2 mScreenSquareMin;
     glm::vec2 mScreenSquareMax;

     float mScreenFactor;
     glm::vec4 mRelativeOrigin;

     bool mbUsing;
     bool mbUsingViewManipulate;
     bool mbEnable;
     bool mbMouseOver;
     bool mReversed; // reversed projection matrix
     bool mIsViewManipulatorHovered;

     // translation
     glm::vec4 mTranslationPlan;
     glm::vec4 mTranslationPlanOrigin;
     glm::vec4 mMatrixOrigin;
     glm::vec4 mTranslationLastDelta;

     // rotation
     glm::vec4 mRotationVectorSource;
     float mRotationAngle;
     float mRotationAngleOrigin;
     //glm::vec4 mWorldToLocalAxis;

     // scale
     glm::vec4 mScale;
     glm::vec4 mScaleValueOrigin;
     glm::vec4 mScaleLast;
     float mSaveMousePosx;

     // save axis factor when using gizmo
     bool mBelowAxisLit[3];
     int mAxisMask = 0;
     bool mBelowPlaneLit[3];
     float mAxisFactor[3];

     float mAxisLit=0.0025f;
     float mPlaneLit=0.02f;

     // bounds stretching
     glm::vec4 mBoundsPivot;
     glm::vec4 mBoundsAnchor;
     glm::vec4 mBoundsPlan;
     glm::vec4 mBoundsLocalPivot;
     int mBoundsBestAxis;
     int mBoundsAxis[2];
     bool mbUsingBounds;
     glm::mat4 mBoundsMatrix;

     //
     MOVETYPE mCurrentHandleType = MT_NONE;
     MOVETYPE mHoveredHandleType = MT_NONE;

     float mX = 0.f;
     float mY = 0.f;
     float mWidth = 0.f;
     float mHeight = 0.f;
     float mXMax = 0.f;
     float mYMax = 0.f;
     float mDisplayRatio = 1.f;

     bool mIsOrthographic = false;
     // check to not have multiple gizmo highlighted at the same te
     bool mbOverGizmoHotspot = false;

     GuiWindow* mAlternativeWindow = nullptr;
     std::vector<WidgetID> mIDStack;
     WidgetID mEditingID = -1;
     OPERATION mOperation = OPERATION(0);

     bool mAllowAxisFlip = true;
     float mGizmoSizeClipSpace = 0.1f;

     inline WidgetID GetCurrentID()
     {
        if (mIDStack.empty())
        {
           mIDStack.push_back(-1);
        }
        return mIDStack.back();
     }
  };

  static Context gContext;

  static const glm::vec4 directionUnary[3] = { makeVect(1.f, 0.f, 0.f), makeVect(0.f, 1.f, 0.f), makeVect(0.f, 0.f, 1.f) };
  static const char* translationInfoMask[] = { "X : %5.3f", "Y : %5.3f", "Z : %5.3f",
     "Y : %5.3f Z : %5.3f", "X : %5.3f Z : %5.3f", "X : %5.3f Y : %5.3f",
     "X : %5.3f Y : %5.3f Z : %5.3f" };
  static const char* scaleInfoMask[] = { "X : %5.2f", "Y : %5.2f", "Z : %5.2f", "XYZ : %5.2f" };
  static const char* rotationInfoMask[] = { "X : %5.2f deg %5.2f rad", "Y : %5.2f deg %5.2f rad", "Z : %5.2f deg %5.2f rad", "Screen : %5.2f deg %5.2f rad" };
  static const int translationInfoIndex[] = { 0,0,0, 1,0,0, 2,0,0, 1,2,0, 0,2,0, 0,1,0, 0,1,2 };
  static const float quadMin = 0.5f;
  static const float quadMax = 0.8f;
  static const float quadUV[8] = { quadMin, quadMin, quadMin, quadMax, quadMax, quadMax, quadMax, quadMin };
  static const int halfCircleSegmentCount = 64;
  static const float snapTension = 0.5f;

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //
  static MOVETYPE GetMoveType(OPERATION op, glm::vec4* gizmoHitProportion);
  static MOVETYPE GetRotateType(OPERATION op);
  static MOVETYPE GetScaleType(OPERATION op);

  static glm::vec2 worldToPos(const glm::vec4& worldPos, const glm::mat4& mat, glm::vec2 position = glm::vec2(gContext.mX, gContext.mY), glm::vec2 size = glm::vec2(gContext.mWidth, gContext.mHeight))
  {
      glm::vec4 trans = mat * glm::vec4(glm::vec3(worldPos), 1.0f);

      if (trans.w < 0.001f)
          return glm::vec2(-FLT_MAX, -FLT_MAX);

      trans.x /= trans.w;
      trans.y /= trans.w;

      trans.x = trans.x * 0.5f + 0.5f;
      trans.y = trans.y * 0.5f + 0.5f;

      trans.y = 1.0f - trans.y;

      trans.x *= size.x;
      trans.y *= size.y;
      trans.x += position.x;
      trans.y += position.y;

      return glm::vec2(trans.x, trans.y);
  }


  static void ComputeCameraRay(glm::vec4& rayOrigin, glm::vec4& rayDir, glm::vec2 position = glm::vec2(gContext.mX, gContext.mY), glm::vec2 size = glm::vec2(gContext.mWidth, gContext.mHeight))
  {
      IInput* input = gContext.mInput;
      if (!input) return;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);

      glm::mat4 viewProj = glm::mat4(gContext.mProjectionMat) * glm::mat4(gContext.mViewMat);
      glm::mat4 mViewProjInverse = glm::inverse(viewProj);

      const float mox = ((static_cast<float>(mouseX) - position.x) / size.x) * 2.f - 1.f;
      const float moy = (1.f - ((static_cast<float>(mouseY) - position.y) / size.y)) * 2.f - 1.f;

      const float zNear = gContext.mReversed ? (1.f - FLT_EPSILON) : 0.f;
      const float zFar = gContext.mReversed ? 0.f : (1.f - FLT_EPSILON);

      glm::vec4 startNDC = glm::vec4(mox, moy, zNear, 1.f);
      glm::vec4 endNDC = glm::vec4(mox, moy, zFar, 1.f);

      glm::vec4 startWorld = mViewProjInverse * startNDC;
      startWorld /= startWorld.w;
      rayOrigin = startWorld;

      glm::vec4 endWorld = mViewProjInverse * endNDC;
      endWorld /= endWorld.w;

      rayDir = glm::vec4(glm::normalize(glm::vec3(endWorld) - glm::vec3(startWorld)), 0.f);
  }

  void DrawTransformAxes(DrawList* drawList, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& transform)
  {
      if (!drawList) return;

      if (drawList->cmdBuffer.empty()) {
          DrawCmd initCmd{ 0, 0, Rect(glm::vec2(0, 0), glm::vec2(gContext.mWidth, gContext.mHeight)) };
          drawList->cmdBuffer.push_back(initCmd);
      }

      glm::mat4 viewProj = projection * view;

      glm::vec4 origin = transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

      glm::vec4 axes[3] = {
          glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), // X
          glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), // Y
          glm::vec4(0.0f, 0.0f, 1.0f, 0.0f)  // Z
      };

      U32 colors[3] = {
          0xFF0000FF, // Red (X)
          0xFF00FF00, // Green (Y)
          0xFFFF0000  // Blue (Z)
      };

      float axisLength = 1.0f;

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < 3; i++) {
          glm::vec4 worldAxis = transform * axes[i];
          worldAxis.w = 0.0f;

          glm::vec4 endPoint = origin + worldAxis * axisLength;

          glm::vec4 clipOrigin = viewProj * origin;
          glm::vec4 clipEnd = viewProj * endPoint;

          if (clipOrigin.w > 0.001f && clipEnd.w > 0.001f) {
              glm::vec2 p0 = worldToPos(origin, viewProj, vpPos, vpSize);
              glm::vec2 p1 = worldToPos(endPoint, viewProj, vpPos, vpSize);

              glm::vec2 line[2] = { p0, p1 };

              drawList->AddPolyline(
                  line,
                  2,
                  colors[i],
                  true,
                  4.0f
              );
          }
      }
  }


  void UpdateContext(const RenderContext& ctx, DrawList& list, IRenderPlatform* platform, IRenderDevice* device)
  {
      gContext.mDrawList = static_cast<DrawList*>(&list);
      gContext.mInput = static_cast<IInput*>(ctx.input);

      glm::vec2 windowSize = glm::vec2(platform->getWidth(), platform->getHeight());
      glm::vec2 mousePos = ctx.input->getMousePosition();

      if (gContext.mDrawList) {
          //gContext.mDrawList->BeginFrame(windowSize);
      }

      auto scene = ctx.scene;
      auto* camTransform = scene->getComponent<Transform>(ctx.activeCamera);
      auto* camComponent = scene->getComponent<CameraComponent>(ctx.activeCamera);

      if (!camTransform || !camComponent)
          return;

      auto viewMatrix = getViewMatrix(*camTransform);
      auto projMatrix = getProjectionMatrix(*camComponent);

      gContext.mX = 0.0f; gContext.mY = 0.0f;
      gContext.mWidth = windowSize.x; gContext.mHeight = windowSize.y;
      gContext.mXMax = windowSize.x; gContext.mYMax = windowSize.y;
      gContext.mDisplayRatio = windowSize.x / windowSize.y;
      gContext.mSaveMousePosx = static_cast<float>(mousePos.x);
      gContext.mbMouseOver = true;

      gContext.mViewMat = viewMatrix;
      gContext.mProjectionMat = projMatrix;
      gContext.mViewProjection = projMatrix * viewMatrix;
      gContext.mIsOrthographic = camComponent->isOrtho;
      gContext.mReversed = false;

      glm::mat4 invView = glm::inverse(viewMatrix);

      gContext.mCameraRight = invView[0];
      gContext.mCameraUp = invView[1];
      gContext.mCameraDir = -invView[2];
      gContext.mCameraEye = invView[3];

      ComputeCameraRay(gContext.mRayOrigin, gContext.mRayVector);
  }

  Style& GetStyle()
  {
     return gContext.mStyle;
  }

  static U32 GetColorU32(int idx)
  {
     _ASSERT(idx < COLOR::COUNT);
     return ColorConvertFloat4ToU32(gContext.mStyle.Colors[idx]);
  }

  static float GetSegmentLengthClipSpace(const glm::vec4& start, const glm::vec4& end, const bool localCoordinates = false)
  {
      glm::mat4 mvp = localCoordinates ? gContext.mMVPLocal : gContext.mMVP;

      glm::vec4 startOfSegment = mvp * glm::vec4(glm::vec3(start), 1.f);
      if (fabsf(startOfSegment.w) > FLT_EPSILON)
      {
          startOfSegment /= startOfSegment.w;
      }

      glm::vec4 endOfSegment = mvp * glm::vec4(glm::vec3(end), 1.f);
      if (fabsf(endOfSegment.w) > FLT_EPSILON)
      {
          endOfSegment /= endOfSegment.w;
      }

      glm::vec4 clipSpaceAxis = endOfSegment - startOfSegment;

      if (gContext.mDisplayRatio < 1.0f)
          clipSpaceAxis.x *= gContext.mDisplayRatio;
      else
          clipSpaceAxis.y /= gContext.mDisplayRatio;

      return sqrtf(clipSpaceAxis.x * clipSpaceAxis.x + clipSpaceAxis.y * clipSpaceAxis.y);
  }

  static float GetParallelogram(const glm::vec4& ptO, const glm::vec4& ptA, const glm::vec4& ptB)
  {
      glm::mat4 mvp = gContext.mMVP;
      glm::vec4 pts[3];

      pts[0] = mvp * glm::vec4(glm::vec3(ptO), 1.f);
      pts[1] = mvp * glm::vec4(glm::vec3(ptA), 1.f);
      pts[2] = mvp * glm::vec4(glm::vec3(ptB), 1.f);

      for (unsigned int i = 0; i < 3; i++)
      {
          if (fabsf(pts[i].w) > FLT_EPSILON)
          {
              pts[i] /= pts[i].w;
          }
      }

      glm::vec4 segA = pts[1] - pts[0];
      glm::vec4 segB = pts[2] - pts[0];

      segA.y /= gContext.mDisplayRatio;
      segB.y /= gContext.mDisplayRatio;

      glm::vec3 segAOrtho = glm::vec3(-segA.y, segA.x, 0.f);
      float len = glm::length(segAOrtho);
      if (len > FLT_EPSILON)
      {
          segAOrtho /= len;
      }

      float dt = glm::dot(segAOrtho, glm::vec3(segB));
      return glm::length(glm::vec3(segA)) * fabsf(dt);
  }

  inline glm::vec4 PointOnSegment(const glm::vec4& point, const glm::vec4& vertPos1, const glm::vec4& vertPos2)
  {
      glm::vec3 c = glm::vec3(point - vertPos1);
      glm::vec3 delta = glm::vec3(vertPos2 - vertPos1);

      float d = glm::length(delta);
      glm::vec3 V = (d > FLT_EPSILON) ? (delta / d) : glm::vec3(0.f);
      float t = glm::dot(V, c);

      if (t < 0.f)
      {
          return vertPos1;
      }

      if (t > d)
      {
          return vertPos2;
      }

      return vertPos1 + glm::vec4(V * t, 0.f);
  }

  static float IntersectRayPlane(const glm::vec4& rOrigin, const glm::vec4& rVector, const glm::vec4& plan)
  {
      const float numer = glm::dot(glm::vec3(plan), glm::vec3(rOrigin)) - plan.w;
      const float denom = glm::dot(glm::vec3(plan), glm::vec3(rVector));

      if (fabsf(denom) < FLT_EPSILON)
      {
          return -1.0f;
      }

      return -(numer / denom);
  }

  static float DistanceToPlane(const glm::vec4& point, const glm::vec4& plan)
  {
      return glm::dot(glm::vec3(plan), glm::vec3(point)) + plan.w;
  }


  static bool IsInContextRect(glm::vec2 p)
  {
      return IsWithin(p.x, gContext.mX, gContext.mXMax) && IsWithin(p.y, gContext.mY, gContext.mYMax);
  }

  static bool IsHoveringWindow()
  {
     //GuiContext& g = *Gui::GetCurrentContext();
     //GuiWindow* window = Gui::FindWindowByName(gContext.mDrawList->_OwnerName);
     //if (g.HoveredWindow == window)   // Mouse hovering drawlist window
     //   return true;
     //if (gContext.mAlternativeWindow != nullptr && g.HoveredWindow == gContext.mAlternativeWindow)
     //   return true;
     //if (g.HoveredWindow != NULL)     // Any other window is hovered
     //   return false;
     //if (Gui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max, false))   // Hovering drawlist window rect, while no other window is hovered (for _NoInputs windows)
     //   return true;
     return false;
  }

  void SetRect(float x, float y, float width, float height)
  {
     gContext.mX = x;
     gContext.mY = y;
     gContext.mWidth = width;
     gContext.mHeight = height;
     gContext.mXMax = gContext.mX + gContext.mWidth;
     gContext.mYMax = gContext.mY + gContext.mXMax;
     gContext.mDisplayRatio = width / height;
  }

  void SetOrthographic(bool isOrthographic)
  {
     gContext.mIsOrthographic = isOrthographic;
  }

  bool IsUsing()
  {
     return (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID)) || gContext.mbUsingBounds;
  }

  bool IsUsingViewManipulate()
  {
     return gContext.mbUsingViewManipulate;
  }

  bool IsViewManipulateHovered()
  {
     return gContext.mIsViewManipulatorHovered;
  }

  bool IsUsingAny()
  {
     return gContext.mbUsing || gContext.mbUsingBounds;
  }

  bool IsOver()
  {
     return (Intersects(gContext.mOperation, TRANSLATE) && GetMoveType(gContext.mOperation, NULL) != MT_NONE) ||
        (Intersects(gContext.mOperation, ROTATE) && GetRotateType(gContext.mOperation) != MT_NONE) ||
        (Intersects(gContext.mOperation, SCALE) && GetScaleType(gContext.mOperation) != MT_NONE) || IsUsing();
  }

  bool IsOver(OPERATION op)
  {
     if(IsUsing())
     {
        return true;
     }
     if(Intersects(op, SCALE) && GetScaleType(op) != MT_NONE)
     {
        return true;
     }
     if(Intersects(op, ROTATE) && GetRotateType(op) != MT_NONE)
     {
        return true;
     }
     if(Intersects(op, TRANSLATE) && GetMoveType(op, NULL) != MT_NONE)
     {
        return true;
     }
     return false;
  }

  MOVETYPE GetActiveHandleType()
  {
     if (!gContext.mbUsing || gContext.GetCurrentID() != gContext.mEditingID)
     {
        return MT_NONE;
     }
     return gContext.mCurrentHandleType;
  }

  MOVETYPE GetHoveredHandleType()
  {
     return gContext.mHoveredHandleType;
  }

  MOVETYPE GetActiveMoveType()
  {
     return GetActiveHandleType();
  }

  MOVETYPE GetHoveredMoveType()
  {
     return GetHoveredHandleType();
  }

  void Enable(bool enable)
  {
     gContext.mbEnable = enable;
     if (!enable)
     {
        gContext.mbUsing = false;
        gContext.mbUsingBounds = false;
        gContext.mCurrentHandleType = MT_NONE;
        gContext.mHoveredHandleType = MT_NONE;
     }
  }

  static void ComputeContext(const float* view, const float* projection, float* matrix, MODE mode)
  {
     gContext.mMode = mode;
     gContext.mViewMat = *(glm::mat4*)view;
     gContext.mProjectionMat = *(glm::mat4*)projection;
     gContext.mbMouseOver = IsHoveringWindow();

     gContext.mModelLocal = *(glm::mat4*)matrix;
     gContext.mModelLocal = *(glm::mat4*)matrix;

     glm::vec3 r = glm::normalize(glm::vec3(gContext.mModelLocal[0]));
     glm::vec3 u = glm::normalize(glm::vec3(gContext.mModelLocal[1]) - glm::dot(r, glm::vec3(gContext.mModelLocal[1])) * r);
     glm::vec3 d = glm::cross(r, u);

     gContext.mModelLocal[0] = glm::vec4(r, 0.0f);
     gContext.mModelLocal[1] = glm::vec4(u, 0.0f);
     gContext.mModelLocal[2] = glm::vec4(d, 0.0f);

     if (mode == LOCAL)
     {
        gContext.mModel = gContext.mModelLocal;
     }
     else
     {
         gContext.mModel = glm::mat4(1.0f);
         gContext.mModel[3] = (*(glm::mat4*)matrix)[3];
     }

     gContext.mModelSource = *(glm::mat4*)matrix;
     gContext.mModelScaleOrigin = glm::vec4(
         glm::length(glm::vec3(gContext.mModelSource[0])),
         glm::length(glm::vec3(gContext.mModelSource[1])),
         glm::length(glm::vec3(gContext.mModelSource[2])),
         0.0f
     );

     gContext.mModelInverse = glm::inverse(gContext.mModel);
     gContext.mModelInverse = glm::inverse(gContext.mModelSource);

     gContext.mViewProjection = gContext.mViewMat * gContext.mProjectionMat;
     gContext.mMVP = gContext.mModel * gContext.mViewProjection;
     gContext.mMVPLocal = gContext.mModelLocal * gContext.mViewProjection;

     glm::mat4 viewInverse;
     viewInverse = glm::inverse(gContext.mViewMat);
     gContext.mCameraDir = viewInverse[2];
     gContext.mCameraEye = viewInverse[3];
     gContext.mCameraRight = viewInverse[0];
     gContext.mCameraUp = viewInverse[1];

     // projection reverse
     glm::vec4 nearPos = gContext.mProjectionMat * glm::vec4(0.0f, 0.0f, 1.f, 1.f);
     glm::vec4 farPos = gContext.mProjectionMat * glm::vec4(0.0f, 0.0f, 2.f, 1.f);

     gContext.mReversed = (nearPos.z / nearPos.w) > (farPos.z / farPos.w);

     // compute scale from the size of camera right vector projected on screen at the matrix position
     glm::vec4 pointRight = glm::vec4(glm::vec3(viewInverse[0]), 1.0f);
     pointRight = gContext.mViewProjection * pointRight;

     glm::vec4 rightViewInverse = glm::vec4(glm::vec3(viewInverse[0]), 0.0f);
     rightViewInverse = gContext.mModelInverse * rightViewInverse;

     float rightLength = GetSegmentLengthClipSpace(makeVect(0.f, 0.f), rightViewInverse);
     gContext.mScreenFactor = gContext.mGizmoSizeClipSpace / rightLength;

     glm::vec2 centerSSpace = worldToPos(makeVect(0.f, 0.f), gContext.mMVP);
     gContext.mScreenSquareCenter = centerSSpace;
     gContext.mScreenSquareMin = glm::vec2(centerSSpace.x - 10.f, centerSSpace.y - 10.f);
     gContext.mScreenSquareMax = glm::vec2(centerSSpace.x + 10.f, centerSSpace.y + 10.f);

     ComputeCameraRay(gContext.mRayOrigin, gContext.mRayVector);
  }

  static void ComputeColors(U32* colors, int type, OPERATION operation)
  {
     if (gContext.mbEnable)
     {
        U32 selectionColor = GetColorU32(SELECTION);

        switch (operation)
        {
        case TRANSLATE:
           colors[0] = (type == MT_MOVE_SCREEN) ? selectionColor : _COL32_WHITE;
           for (int i = 0; i < 3; i++)
           {
              colors[i + 1] = (type == (int)(MT_MOVE_X + i)) ? selectionColor : GetColorU32(DIRECTION_X + i);
              colors[i + 4] = (type == (int)(MT_MOVE_YZ + i)) ? selectionColor : GetColorU32(PLANE_X + i);
              colors[i + 4] = (type == MT_MOVE_SCREEN) ? selectionColor : colors[i + 4];
           }
           break;
        case ROTATE:
           colors[0] = (type == MT_ROTATE_SCREEN) ? selectionColor : _COL32_WHITE;
           for (int i = 0; i < 3; i++)
           {
              colors[i + 1] = (type == (int)(MT_ROTATE_X + i)) ? selectionColor : GetColorU32(DIRECTION_X + i);
           }
           break;
        case SCALEU:
        case SCALE:
           colors[0] = (type == MT_SCALE_XYZ) ? selectionColor : _COL32_WHITE;
           for (int i = 0; i < 3; i++)
           {
              colors[i + 1] = (type == (int)(MT_SCALE_X + i)) ? selectionColor : GetColorU32(DIRECTION_X + i);
           }
           break;
        // note: this internal function is only called with three possible values for operation
        default:
           break;
        }
     }
     else
     {
        U32 inactiveColor = GetColorU32(INACTIVE);
        for (int i = 0; i < 7; i++)
        {
           colors[i] = inactiveColor;
        }
     }
  }

  static void ComputeTripodAxisAndVisibility(const int axisIndex, glm::vec4& dirAxis, glm::vec4& dirPlaneX, glm::vec4& dirPlaneY, bool& belowAxisLit, bool& belowPlaneLit, const bool localCoordinates = false)
  {
     dirAxis = directionUnary[axisIndex];
     dirPlaneX = directionUnary[(axisIndex + 1) % 3];
     dirPlaneY = directionUnary[(axisIndex + 2) % 3];

     if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID))
     {
        // when using, use stored factors so the gizmo doesn't flip when we translate

        // Apply axis mask to axes and planes
        belowAxisLit = gContext.mBelowAxisLit[axisIndex] && ((1<<axisIndex)&gContext.mAxisMask);
        belowPlaneLit = gContext.mBelowPlaneLit[axisIndex] && (((1<<axisIndex) == gContext.mAxisMask) || !gContext.mAxisMask);

        dirAxis *= gContext.mAxisFactor[axisIndex];
        dirPlaneX *= gContext.mAxisFactor[(axisIndex + 1) % 3];
        dirPlaneY *= gContext.mAxisFactor[(axisIndex + 2) % 3];
     }
     else
     {
        // new method
        float lenDir = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirAxis, localCoordinates);
        float lenDirMinus = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirAxis, localCoordinates);

        float lenDirPlaneX = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirPlaneX, localCoordinates);
        float lenDirMinusPlaneX = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirPlaneX, localCoordinates);

        float lenDirPlaneY = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirPlaneY, localCoordinates);
        float lenDirMinusPlaneY = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), -dirPlaneY, localCoordinates);

        // For readability
        bool & allowFlip = gContext.mAllowAxisFlip;
        float mulAxis = (allowFlip && lenDir < lenDirMinus&& fabsf(lenDir - lenDirMinus) > FLT_EPSILON) ? -1.f : 1.f;
        float mulAxisX = (allowFlip && lenDirPlaneX < lenDirMinusPlaneX&& fabsf(lenDirPlaneX - lenDirMinusPlaneX) > FLT_EPSILON) ? -1.f : 1.f;
        float mulAxisY = (allowFlip && lenDirPlaneY < lenDirMinusPlaneY&& fabsf(lenDirPlaneY - lenDirMinusPlaneY) > FLT_EPSILON) ? -1.f : 1.f;
        dirAxis *= mulAxis;
        dirPlaneX *= mulAxisX;
        dirPlaneY *= mulAxisY;

        // for axis
        float axisLengthInClipSpace = GetSegmentLengthClipSpace(makeVect(0.f, 0.f, 0.f), dirAxis * gContext.mScreenFactor, localCoordinates);

        float paraSurf = GetParallelogram(makeVect(0.f, 0.f, 0.f), dirPlaneX * gContext.mScreenFactor, dirPlaneY * gContext.mScreenFactor);
        // Apply axis mask to axes and planes
        belowPlaneLit = (paraSurf > gContext.mAxisLit) && (((1<<axisIndex) == gContext.mAxisMask) || !gContext.mAxisMask);
        belowAxisLit = (axisLengthInClipSpace > gContext.mPlaneLit) && !((1<<axisIndex)&gContext.mAxisMask);

        // and store values
        gContext.mAxisFactor[axisIndex] = mulAxis;
        gContext.mAxisFactor[(axisIndex + 1) % 3] = mulAxisX;
        gContext.mAxisFactor[(axisIndex + 2) % 3] = mulAxisY;
        gContext.mBelowAxisLit[axisIndex] = belowAxisLit;
        gContext.mBelowPlaneLit[axisIndex] = belowPlaneLit;
     }
  }

  static void ComputeSnap(float* value, float snap)
  {
     if (snap <= FLT_EPSILON)
     {
        return;
     }

     float modulo = fmodf(*value, snap);
     float moduloRatio = fabsf(modulo) / snap;
     if (moduloRatio < snapTension)
     {
        *value -= modulo;
     }
     else if (moduloRatio > (1.f - snapTension))
     {
        *value = *value - modulo + snap * ((*value < 0.f) ? -1.f : 1.f);
     }
  }
  static void ComputeSnap(glm::vec4& value, const float* snap)
  {
     for (int i = 0; i < 3; i++)
     {
        ComputeSnap(&value[i], snap[i]);
     }
  }
  
  static float ComputeAngleOnPlan()
  {
      const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
      glm::vec3 intersectPt = glm::vec3(gContext.mRayOrigin + gContext.mRayVector * len) - glm::vec3(gContext.mModel[3]);
      glm::vec3 localPos = (glm::length(intersectPt) > FLT_EPSILON) ? glm::normalize(intersectPt) : glm::vec3(0.f);

      glm::vec3 perpendicularVector = glm::cross(glm::vec3(gContext.mRotationVectorSource), glm::vec3(gContext.mTranslationPlan));
      float lenPerp = glm::length(perpendicularVector);
      if (lenPerp > FLT_EPSILON)
      {
          perpendicularVector /= lenPerp;
      }

      float acosAngle = glm::clamp(glm::dot(localPos, glm::vec3(gContext.mRotationVectorSource)), -1.f, 1.f);
      float angle = acosf(acosAngle);
      angle *= (glm::dot(localPos, perpendicularVector) < 0.f) ? 1.f : -1.f;
      return angle;
  }

  static void DrawRotationGizmo(OPERATION op, MOVETYPE type)
  {
     if(!Intersects(op, ROTATE))
     {
        return;
     }
     DrawList* drawList = gContext.mDrawList;

     bool isMultipleAxesMasked = (gContext.mAxisMask & (gContext.mAxisMask - 1)) != 0;
     bool isNoAxesMasked = !gContext.mAxisMask;

     // colors
     U32 colors[7];
     ComputeColors(colors, type, ROTATE);

     glm::vec4 viewDirNormalized;
     glm::mat4 viewInverse;
     if (gContext.mIsOrthographic)
     {
         viewInverse = glm::inverse(gContext.mViewMat);
         viewDirNormalized = -viewInverse[2];
     }
     else
     {
         viewInverse = glm::inverse(gContext.mViewMat);
         glm::vec3 right = glm::vec3(viewInverse[0]);
         glm::vec3 up = glm::vec3(viewInverse[1]);
         glm::vec3 dir = glm::vec3(viewInverse[2]);
         const float handSign = (glm::dot(glm::cross(right, up), dir) >= 0.f) ? 1.f : -1.f;
         viewDirNormalized = glm::vec4(glm::normalize(glm::vec3(gContext.mCameraDir)) * handSign, 0.f);
     }

     viewDirNormalized = gContext.mModelInverse * glm::vec4(glm::vec3(viewDirNormalized), 0.f);

     gContext.mRadiusSquareCenter = screenRotateSize * gContext.mHeight;

     bool hasRSC = Intersects(op, ROTATE_SCREEN);
     for (int axis = 0; axis < 3; axis++)
     {
        if(!Intersects(op, static_cast<OPERATION>(ROTATE_Z >> axis)))
        {
           continue;
        }

        bool isAxisMasked = ((1 << (2 - axis)) & gContext.mAxisMask) != 0;

        if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked)
        {
           continue;
        }
        const bool usingAxis = (gContext.mbUsing && type == MT_ROTATE_Z - axis);
        const int circleMul = (hasRSC && !usingAxis) ? 1 : 2;

        glm::vec2* circlePos = (glm::vec2*)alloca(sizeof(glm::vec2) * (circleMul * halfCircleSegmentCount + 1));
        const bool rightHanded = gContext.mProjectionMat[2][3] < 0.f;
        float angleStart = atan2f(viewDirNormalized[(4 - axis) % 3], viewDirNormalized[(3 - axis) % 3]) + (gContext.mIsOrthographic ? ZPI : -ZPI) * 0.5f + (rightHanded ? 0.f : ZPI);

        for (int i = 0; i < circleMul * halfCircleSegmentCount + 1; i++)
        {
           float ng = angleStart + (float)circleMul * ZPI * ((float)i / (float)(circleMul * halfCircleSegmentCount));
           glm::vec4 axisPos = makeVect(cosf(ng), sinf(ng), 0.f);
           glm::vec4 pos = makeVect(axisPos[axis], axisPos[(axis + 1) % 3], axisPos[(axis + 2) % 3]) * gContext.mScreenFactor * rotationDisplayFactor;
           circlePos[i] = worldToPos(pos, gContext.mMVP);
        }
        if (!gContext.mbUsing || usingAxis)
        {
           drawList->AddPolyline(circlePos, circleMul* halfCircleSegmentCount + 1, colors[3 - axis], gContext.mStyle.RotationLineThickness, 0);
        }

        float radiusAxis = sqrtf((glm::length2(worldToPos(gContext.mModel[3], gContext.mViewProjection) - circlePos[0])));
        if (radiusAxis > gContext.mRadiusSquareCenter)
        {
           gContext.mRadiusSquareCenter = radiusAxis;
        }
     }
     if(hasRSC && (!gContext.mbUsing || type == MT_ROTATE_SCREEN) && (!isMultipleAxesMasked && isNoAxesMasked))
     {
        drawList->AddCircle(worldToPos(gContext.mModel[3], gContext.mViewProjection), gContext.mRadiusSquareCenter, colors[0], 64, gContext.mStyle.RotationOuterLineThickness);
     }

     if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsRotateType(type))
     {
        glm::vec2 circlePos[halfCircleSegmentCount + 1];

        circlePos[0] = worldToPos(gContext.mModel[3], gContext.mViewProjection);
        for (unsigned int i = 1; i < halfCircleSegmentCount + 1; i++)
        {
            float ng = gContext.mRotationAngle * ((float)(i - 1) / (float)(halfCircleSegmentCount - 1));

            glm::mat4 rotateVectorMatrix = glm::rotate(glm::mat4(1.0f), ng, glm::vec3(gContext.mTranslationPlan));

            glm::vec4 pos = rotateVectorMatrix * glm::vec4(glm::vec3(gContext.mRotationVectorSource), 1.0f);
            if (fabsf(pos.w) > FLT_EPSILON)
            {
                pos /= pos.w;
            }

            pos = glm::vec4(glm::vec3(pos) * (gContext.mScreenFactor * rotationDisplayFactor), 1.0f);

            glm::vec4 worldPos = pos + glm::vec4(glm::vec3(gContext.mModel[3]), 0.0f);

            circlePos[i] = worldToPos(worldPos, gContext.mViewProjection, glm::vec2(gContext.mX, gContext.mY), glm::vec2(gContext.mWidth, gContext.mHeight));
        }

        drawList->AddConvexPolyFilled(circlePos, halfCircleSegmentCount + 1, GetColorU32(ROTATION_USING_FILL));
        drawList->AddPolyline(circlePos, halfCircleSegmentCount + 1, GetColorU32(ROTATION_USING_BORDER), gContext.mStyle.RotationLineThickness, DrawFlags_Closed);

        glm::vec2 destinationPosOnScreen = circlePos[1];
        char tmps[512];
        FormatString(tmps, sizeof(tmps), rotationInfoMask[type - MT_ROTATE_X], (gContext.mRotationAngle / ZPI) * 180.f, gContext.mRotationAngle);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), GetColorU32(TEXT_SHADOW), tmps);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), GetColorU32(TEXT), tmps);
     }
  }

  static void DrawHatchedAxis(const glm::vec4& axis)
  {
     if (gContext.mStyle.HatchedAxisLineThickness <= 0.0f)
     {
        return;
     }

     for (int j = 1; j < 10; j++)
     {
        glm::vec2 baseSSpace2 = worldToPos(axis * 0.05f * (float)(j * 2) * gContext.mScreenFactor, gContext.mMVP);
        glm::vec2 worldDirSSpace2 = worldToPos(axis * 0.05f * (float)(j * 2 + 1) * gContext.mScreenFactor, gContext.mMVP);
        gContext.mDrawList->AddLine(baseSSpace2, worldDirSSpace2, GetColorU32(HATCHED_AXIS_LINES), gContext.mStyle.HatchedAxisLineThickness);
     }
  }

  static void DrawScaleGizmo(OPERATION op, MOVETYPE type)
  {
      DrawList* drawList = gContext.mDrawList;

      if (!Intersects(op, SCALE))
      {
          return;
      }

      U32 colors[7];
      ComputeColors(colors, type, SCALE);

      glm::vec4 scaleDisplay = glm::vec4(1.f, 1.f, 1.f, 1.f);

      if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID))
      {
          scaleDisplay = gContext.mScale;
      }

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < 3; i++)
      {
          if (!Intersects(op, static_cast<OPERATION>(SCALE_X << i)))
          {
              continue;
          }
          const bool usingAxis = (gContext.mbUsing && type == MT_SCALE_X + i);
          if (!gContext.mbUsing || usingAxis)
          {
              glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
              bool belowAxisLit, belowPlaneLit;
              ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit, true);

              if (belowAxisLit)
              {
                  bool hasTranslateOnAxis = Contains(op, static_cast<OPERATION>(TRANSLATE_X << i));
                  float markerScale = hasTranslateOnAxis ? 1.4f : 1.0f;
                  glm::vec2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVP, vpPos, vpSize);
                  glm::vec2 worldDirSSpaceNoScale = worldToPos(dirAxis * markerScale * gContext.mScreenFactor, gContext.mMVP, vpPos, vpSize);
                  glm::vec2 worldDirSSpace = worldToPos((dirAxis * markerScale * scaleDisplay[i]) * gContext.mScreenFactor, gContext.mMVP, vpPos, vpSize);

                  if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID))
                  {
                      U32 scaleLineColor = GetColorU32(SCALE_LINE);
                      drawList->AddLine(baseSSpace, worldDirSSpaceNoScale, scaleLineColor, gContext.mStyle.ScaleLineThickness);
                      drawList->AddCircleFilled(worldDirSSpaceNoScale, gContext.mStyle.ScaleLineCircleSize, scaleLineColor);
                  }

                  if (!hasTranslateOnAxis || gContext.mbUsing)
                  {
                      drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], gContext.mStyle.ScaleLineThickness);
                  }
                  drawList->AddCircleFilled(worldDirSSpace, gContext.mStyle.ScaleLineCircleSize, colors[i + 1]);

                  if (gContext.mAxisFactor[i] < 0.f)
                  {
                      DrawHatchedAxis(dirAxis * scaleDisplay[i]);
                  }
              }
          }
      }

      drawList->AddCircleFilled(gContext.mScreenSquareCenter, gContext.mStyle.CenterCircleSize, colors[0], 32);

      if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsScaleType(type))
      {
          glm::vec2 sourcePosOnScreen = worldToPos(gContext.mMatrixOrigin, gContext.mViewProjection, vpPos, vpSize);
          glm::vec2 destinationPosOnScreen = worldToPos(glm::vec4(glm::vec3(gContext.mModel[3]), 1.f), gContext.mViewProjection, vpPos, vpSize);
          glm::vec3 dif3 = glm::vec3(destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y, 0.f);
          if (glm::length(dif3) > FLT_EPSILON)
          {
              dif3 = glm::normalize(dif3);
          }
          glm::vec2 dif = glm::vec2(dif3.x, dif3.y) * 5.f;

          U32 translationLineColor = GetColorU32(TRANSLATION_LINE);
          drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
          drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
          drawList->AddLine(glm::vec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), glm::vec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);

          char tmps[512];
          int componentInfoIndex = (type - MT_SCALE_X) * 3;
          FormatString(tmps, sizeof(tmps), scaleInfoMask[type - MT_SCALE_X], scaleDisplay[translationInfoIndex[componentInfoIndex]]);
          drawList->AddText(glm::vec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), GetColorU32(TEXT_SHADOW), tmps);
          drawList->AddText(glm::vec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), GetColorU32(TEXT), tmps);
      }
  }

  static void DrawScaleUniveralGizmo(OPERATION op, MOVETYPE type)
  {
     DrawList* drawList = gContext.mDrawList;

     if (!Intersects(op, SCALEU))
     {
        return;
     }

     // colors
     U32 colors[7];
     ComputeColors(colors, type, SCALEU);

     // draw
     glm::vec4 scaleDisplay = { 1.f, 1.f, 1.f, 1.f };

     if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID))
     {
        scaleDisplay = gContext.mScale;
     }

     for (int i = 0; i < 3; i++)
     {
        if (!Intersects(op, static_cast<OPERATION>(SCALE_XU << i)))
        {
           continue;
        }
        const bool usingAxis = (gContext.mbUsing && type == MT_SCALE_X + i);
        if (!gContext.mbUsing || usingAxis)
        {
           glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
           bool belowAxisLit, belowPlaneLit;
           ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit, true);

           // draw axis
           if (belowAxisLit)
           {
              bool hasTranslateOnAxis = Contains(op, static_cast<OPERATION>(TRANSLATE_X << i));
              float markerScale = hasTranslateOnAxis ? 1.4f : 1.0f;
              glm::vec2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVPLocal);
              glm::vec2 worldDirSSpaceNoScale = worldToPos(dirAxis * markerScale * gContext.mScreenFactor, gContext.mMVP);
              glm::vec2 worldDirSSpace = worldToPos((dirAxis * markerScale * scaleDisplay[i]) * gContext.mScreenFactor, gContext.mMVPLocal);

#if 0
              if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID))
              {
                 drawList->AddLine(baseSSpace, worldDirSSpaceNoScale, _COL32(0x40, 0x40, 0x40, 0xFF), 3.f);
                 drawList->AddCircleFilled(worldDirSSpaceNoScale, 6.f, _COL32(0x40, 0x40, 0x40, 0xFF));
              }
              
              if (!hasTranslateOnAxis || gContext.mbUsing)
              {
                 drawList->AddLine(baseSSpace, worldDirSSpace, colors[i + 1], 3.f);
              }
              
#endif
              drawList->AddCircleFilled(worldDirSSpace, 12.f, colors[i + 1]);
           }
        }
     }

     // draw screen cirle
     drawList->AddCircle(gContext.mScreenSquareCenter, 20.f, colors[0], 32, gContext.mStyle.CenterCircleSize);

     if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsScaleType(type))
     {
        glm::vec2 sourcePosOnScreen = worldToPos(gContext.mMatrixOrigin, gContext.mViewProjection);
        glm::vec2 destinationPosOnScreen = worldToPos(gContext.mModel[3], gContext.mViewProjection);
        glm::vec3 dif3 = glm::vec3(destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y, 0.f);
        if (glm::length(dif3) > FLT_EPSILON)
        {
            dif3 = glm::normalize(dif3);
        }
        glm::vec2 dif = glm::vec2(dif3.x, dif3.y) * 5.f;
        U32 translationLineColor = GetColorU32(TRANSLATION_LINE);

        drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
        drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
        drawList->AddLine(glm::vec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), glm::vec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);
        
        char tmps[512];
        glm::vec4 deltaInfo = gContext.mModel[3] - gContext.mMatrixOrigin;
        int componentInfoIndex = (type - MT_SCALE_X) * 3;
        FormatString(tmps, sizeof(tmps), scaleInfoMask[type - MT_SCALE_X], scaleDisplay[translationInfoIndex[componentInfoIndex]]);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), GetColorU32(TEXT_SHADOW), tmps);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), GetColorU32(TEXT), tmps);
     }
  }

  static void DrawTranslationGizmo(OPERATION op, MOVETYPE type)
  {
     DrawList* drawList = gContext.mDrawList;
     if (!drawList)
     {
        return;
     }

     if(!Intersects(op, TRANSLATE))
     {
        return;
     }

     // colors
     U32 colors[7];
     ComputeColors(colors, type, TRANSLATE);

     const glm::vec2 origin = worldToPos(gContext.mModel[3], gContext.mViewProjection);

     // draw
     bool belowAxisLit = false;
     bool belowPlaneLit = false;
     for (int i = 0; i < 3; ++i)
     {
        glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
        ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit);

        if (!gContext.mbUsing || (gContext.mbUsing && type == MT_MOVE_X + i))
        {
           // draw axis
           if (belowAxisLit && Intersects(op, static_cast<OPERATION>(TRANSLATE_X << i)))
           {
              glm::vec2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVP);
              glm::vec2 worldDirSSpace = worldToPos(dirAxis * gContext.mScreenFactor, gContext.mMVP);

              drawList->AddPolyline(&baseSSpace, baseSSpace.length(), colors[i + 1], gContext.mStyle.TranslationLineThickness, 1.0f);

              // Arrow head begin
              glm::vec2 dir(origin - worldDirSSpace);

              float d = sqrtf(glm::length2(dir));
              dir /= d; // Normalize
              dir *= gContext.mStyle.TranslationLineArrowSize;

              glm::vec2 ortogonalDir(dir.y, -dir.x); // Perpendicular vector
              glm::vec2 a(worldDirSSpace + dir);
              drawList->AddTriangleFilled(worldDirSSpace - dir, a + ortogonalDir, a - ortogonalDir, colors[i + 1]);
              // Arrow head end

              if (gContext.mAxisFactor[i] < 0.f)
              {
                 DrawHatchedAxis(dirAxis);
              }
           }
        }
        // draw plane
        if (!gContext.mbUsing || (gContext.mbUsing && type == MT_MOVE_YZ + i))
        {
           if (belowPlaneLit && Contains(op, TRANSLATE_PLANS[i]))
           {
              glm::vec2 screenQuadPts[4];
              for (int j = 0; j < 4; ++j)
              {
                 glm::vec4 cornerWorldPos = (dirPlaneX * quadUV[j * 2] + dirPlaneY * quadUV[j * 2 + 1]) * gContext.mScreenFactor;
                 screenQuadPts[j] = worldToPos(cornerWorldPos, gContext.mMVP);
              }
              drawList->AddPolyline(screenQuadPts, 4, GetColorU32(DIRECTION_X + i), 1.0f, DrawFlags_Closed);
              drawList->AddConvexPolyFilled(screenQuadPts, 4, colors[i + 4]);
           }
        }
     }

     drawList->AddCircleFilled(gContext.mScreenSquareCenter, gContext.mStyle.CenterCircleSize, colors[0], 32);

     if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsTranslateType(type))
     {
        U32 translationLineColor = GetColorU32(TRANSLATION_LINE);

        glm::vec2 sourcePosOnScreen = worldToPos(gContext.mMatrixOrigin, gContext.mViewProjection);
        glm::vec2 destinationPosOnScreen = worldToPos(gContext.mModel[3], gContext.mViewProjection);
        glm::vec4 dif = { destinationPosOnScreen.x - sourcePosOnScreen.x, destinationPosOnScreen.y - sourcePosOnScreen.y, 0.f, 0.f };
        glm::normalize(dif);
        dif *= 5.f;
        drawList->AddCircle(sourcePosOnScreen, 6.f, translationLineColor);
        drawList->AddCircle(destinationPosOnScreen, 6.f, translationLineColor);
        drawList->AddLine(glm::vec2(sourcePosOnScreen.x + dif.x, sourcePosOnScreen.y + dif.y), glm::vec2(destinationPosOnScreen.x - dif.x, destinationPosOnScreen.y - dif.y), translationLineColor, 2.f);

        char tmps[512];
        glm::vec4 deltaInfo = gContext.mModel[3] - gContext.mMatrixOrigin;
        int componentInfoIndex = (type - MT_MOVE_X) * 3;
        FormatString(tmps, sizeof(tmps), translationInfoMask[type - MT_MOVE_X], deltaInfo[translationInfoIndex[componentInfoIndex]], deltaInfo[translationInfoIndex[componentInfoIndex + 1]], deltaInfo[translationInfoIndex[componentInfoIndex + 2]]);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), GetColorU32(TEXT_SHADOW), tmps);
        drawList->AddText(glm::vec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), GetColorU32(TEXT), tmps);
     }
  }

  static bool CanActivate()
  {
      if (!gContext.mbEnable) return false;
      if (gContext.mbUsing) return true;

      IInput* input = gContext.mInput;
      if (input->getMouseButtonDown(MouseButton::Left))
      {
          return gContext.mHoveredHandleType != MT_NONE;
      }

      return false;
  }

  static bool HandleAndDrawLocalBounds(const float* bounds, glm::mat4* matrix, const float* snapValues, OPERATION operation)
  {
      auto input = gContext.mInput;
      if (!input) return false;

      double mx, my;
      input->getMousePosition(mx, my);
      glm::vec2 mousePos(static_cast<float>(mx), static_cast<float>(my));
      DrawList* drawList = gContext.mDrawList;

      bool manipulated = false;

      glm::vec4 axesWorldDirections[3];
      glm::vec4 bestAxisWorldDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
      int axes[3];
      unsigned int numAxes = 1;
      axes[0] = gContext.mBoundsBestAxis;
      int bestAxis = axes[0];
      if (!gContext.mbUsingBounds)
      {
          numAxes = 0;
          float bestDot = 0.f;
          for (int i = 0; i < 3; i++)
          {
              glm::vec4 dirPlaneNormalWorld = gContext.mModelSource * glm::vec4(glm::vec3(directionUnary[i]), 0.0f);
              if (glm::length(glm::vec3(dirPlaneNormalWorld)) > FLT_EPSILON)
              {
                  dirPlaneNormalWorld = glm::vec4(glm::normalize(glm::vec3(dirPlaneNormalWorld)), 0.0f);
              }

              glm::vec3 camToModel = glm::vec3(gContext.mCameraEye) - glm::vec3(gContext.mModelSource[3]);
              if (glm::length(camToModel) > FLT_EPSILON)
              {
                  camToModel = glm::normalize(camToModel);
              }

              float dt = fabsf(glm::dot(camToModel, glm::vec3(dirPlaneNormalWorld)));

              if (dt >= bestDot)
              {
                  bestDot = dt;
                  bestAxis = i;
                  bestAxisWorldDirection = dirPlaneNormalWorld;
              }

              if (dt >= 0.1f)
              {
                  axes[numAxes] = i;
                  axesWorldDirections[numAxes] = dirPlaneNormalWorld;
                  ++numAxes;
              }
          }
      }

      if (numAxes == 0)
      {
          axes[0] = bestAxis;
          axesWorldDirections[0] = bestAxisWorldDirection;
          numAxes = 1;
      }
      else if (bestAxis != axes[0])
      {
          unsigned int bestIndex = 0;
          for (unsigned int i = 0; i < numAxes; i++)
          {
              if (axes[i] == bestAxis)
              {
                  bestIndex = i;
                  break;
              }
          }
          int tempAxis = axes[0];
          axes[0] = axes[bestIndex];
          axes[bestIndex] = tempAxis;
          glm::vec4 tempDirection = axesWorldDirections[0];
          axesWorldDirections[0] = axesWorldDirections[bestIndex];
          axesWorldDirections[bestIndex] = tempDirection;
      }

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (unsigned int axisIndex = 0; axisIndex < numAxes; ++axisIndex)
      {
          bestAxis = axes[axisIndex];
          bestAxisWorldDirection = axesWorldDirections[axisIndex];

          glm::vec4 aabb[4];

          int secondAxis = (bestAxis + 1) % 3;
          int thirdAxis = (bestAxis + 2) % 3;

          for (int i = 0; i < 4; i++)
          {
              aabb[i] = glm::vec4(0.0f);
              aabb[i][secondAxis] = bounds[secondAxis + 3 * (i >> 1)];
              aabb[i][thirdAxis] = bounds[thirdAxis + 3 * ((i >> 1) ^ (i & 1))];
          }

          U32 anchorAlpha = gContext.mbEnable ? 0xFF000000 : 0x80000000;
          glm::mat4 boundsMVP = gContext.mViewProjection * gContext.mModelSource;

          for (int i = 0; i < 4; i++)
          {
              glm::vec4 p0 = boundsMVP * glm::vec4(glm::vec3(aabb[i]), 1.0f);
              glm::vec4 p1 = boundsMVP * glm::vec4(glm::vec3(aabb[(i + 1) % 4]), 1.0f);
              const float wEps = 1e-3f;
              bool in0 = p0.w >= wEps;
              bool in1 = p1.w >= wEps;
              if (!in0 && !in1)
              {
                  continue;
              }
              if (!in0)
              {
                  float t = (p1.w - wEps) / (p1.w - p0.w);
                  p0.x = p1.x + (p0.x - p1.x) * t;
                  p0.y = p1.y + (p0.y - p1.y) * t;
                  p0.z = p1.z + (p0.z - p1.z) * t;
                  p0.w = wEps;
              }
              else if (!in1)
              {
                  float t = (p0.w - wEps) / (p0.w - p1.w);
                  p1.x = p0.x + (p1.x - p0.x) * t;
                  p1.y = p0.y + (p1.y - p0.y) * t;
                  p1.z = p0.z + (p1.z - p0.z) * t;
                  p1.w = wEps;
              }

              auto clipToScreen = [](const glm::vec4& c) -> glm::vec2
                  {
                      float nx = c.x * (0.5f / c.w) + 0.5f;
                      float ny = c.y * (0.5f / c.w) + 0.5f;
                      ny = 1.f - ny;
                      return glm::vec2(gContext.mX + nx * gContext.mWidth, gContext.mY + ny * gContext.mHeight);
                  };

              glm::vec2 worldBound1 = clipToScreen(p0);
              glm::vec2 worldBound2 = clipToScreen(p1);
              float boundDistance = glm::distance(worldBound1, worldBound2);
              int stepCount = (int)(boundDistance / 10.f);
              stepCount = glm::clamp(stepCount, 0, 1000);

              for (int j = 0; j < stepCount; j++)
              {
                  float stepLength = 1.f / (float)stepCount;
                  float t1 = (float)j * stepLength;
                  float t2 = (float)j * stepLength + stepLength * 0.5f;
                  glm::vec2 worldBoundSS1 = glm::mix(worldBound1, worldBound2, glm::vec2(t1));
                  glm::vec2 worldBoundSS2 = glm::mix(worldBound1, worldBound2, glm::vec2(t2));
                  drawList->AddLine(worldBoundSS1, worldBoundSS2, 0xAAAAAA00 + anchorAlpha, 2.f);
              }

              glm::vec4 midPoint = (aabb[i] + aabb[(i + 1) % 4]) * 0.5f;
              glm::vec4 pCorner = boundsMVP * glm::vec4(glm::vec3(aabb[i]), 1.0f);
              glm::vec4 pMid = boundsMVP * glm::vec4(glm::vec3(midPoint), 1.0f);

              glm::vec2 worldBoundOrig = worldToPos(glm::vec4(glm::vec3(aabb[i]), 1.0f), boundsMVP, vpPos, vpSize);
              glm::vec2 midBound = worldToPos(glm::vec4(glm::vec3(midPoint), 1.0f), boundsMVP, vpPos, vpSize);

              bool bigAnchorVisible = pCorner.w >= wEps && IsInContextRect(worldBoundOrig);
              bool smallAnchorVisible = pMid.w >= wEps && IsInContextRect(midBound);

              static const float AnchorBigRadius = 8.f;
              static const float AnchorSmallRadius = 6.f;
              bool overBigAnchor = bigAnchorVisible && glm::length2(worldBoundOrig - mousePos) <= (AnchorBigRadius * AnchorBigRadius);
              bool overSmallAnchor = smallAnchorVisible && glm::length2(midBound - mousePos) <= (AnchorBigRadius * AnchorBigRadius);

              MOVETYPE type = MT_NONE;
              glm::vec4 gizmoHitProportion;

              if (Intersects(operation, TRANSLATE))
              {
                  type = GetMoveType(operation, &gizmoHitProportion);
              }
              if (Intersects(operation, ROTATE) && type == MT_NONE)
              {
                  type = GetRotateType(operation);
              }
              if (Intersects(operation, SCALE) && type == MT_NONE)
              {
                  type = GetScaleType(operation);
              }

              if (type != MT_NONE)
              {
                  overBigAnchor = false;
                  overSmallAnchor = false;
              }

              U32 selectionColor = GetColorU32(SELECTION);
              unsigned int bigAnchorColor = overBigAnchor ? selectionColor : (0xAAAAAA00 + anchorAlpha);
              unsigned int smallAnchorColor = overSmallAnchor ? selectionColor : (0xAAAAAA00 + anchorAlpha);

              if (bigAnchorVisible)
              {
                  drawList->AddCircleFilled(worldBoundOrig, AnchorBigRadius, 0xFF000000);
                  drawList->AddCircleFilled(worldBoundOrig, AnchorBigRadius - 1.2f, bigAnchorColor);
              }

              if (smallAnchorVisible)
              {
                  drawList->AddCircleFilled(midBound, AnchorSmallRadius, 0xFF000000);
                  drawList->AddCircleFilled(midBound, AnchorSmallRadius - 1.2f, smallAnchorColor);
              }
              int oppositeIndex = (i + 2) % 4;

              if (!gContext.mbUsingBounds && gContext.mbEnable && overBigAnchor && CanActivate())
              {
                  gContext.mBoundsPivot = gContext.mModelSource * glm::vec4(glm::vec3(aabb[(i + 2) % 4]), 1.0f);
                  gContext.mBoundsAnchor = gContext.mModelSource * glm::vec4(glm::vec3(aabb[i]), 1.0f);
                  gContext.mBoundsPlan = BuildPlan(gContext.mBoundsAnchor, bestAxisWorldDirection);
                  gContext.mBoundsBestAxis = bestAxis;
                  gContext.mBoundsAxis[0] = secondAxis;
                  gContext.mBoundsAxis[1] = thirdAxis;

                  gContext.mBoundsLocalPivot = glm::vec4(0.0f);
                  gContext.mBoundsLocalPivot[secondAxis] = aabb[oppositeIndex][secondAxis];
                  gContext.mBoundsLocalPivot[thirdAxis] = aabb[oppositeIndex][thirdAxis];

                  gContext.mbUsingBounds = true;
                  gContext.mEditingID = gContext.GetCurrentID();
                  gContext.mBoundsMatrix = gContext.mModelSource;
              }

              if (!gContext.mbUsingBounds && gContext.mbEnable && overSmallAnchor && CanActivate())
              {
                  glm::vec4 midPointOpposite = (aabb[(i + 2) % 4] + aabb[(i + 3) % 4]) * 0.5f;
                  gContext.mBoundsPivot = gContext.mModelSource * glm::vec4(glm::vec3(midPointOpposite), 1.0f);
                  gContext.mBoundsAnchor = gContext.mModelSource * glm::vec4(glm::vec3(midPoint), 1.0f);
                  gContext.mBoundsPlan = BuildPlan(gContext.mBoundsAnchor, bestAxisWorldDirection);
                  gContext.mBoundsBestAxis = bestAxis;
                  int indices[] = { secondAxis , thirdAxis };
                  gContext.mBoundsAxis[0] = indices[i % 2];
                  gContext.mBoundsAxis[1] = -1;

                  gContext.mBoundsLocalPivot = glm::vec4(0.0f);
                  gContext.mBoundsLocalPivot[gContext.mBoundsAxis[0]] = aabb[oppositeIndex][indices[i % 2]];

                  gContext.mbUsingBounds = true;
                  gContext.mEditingID = gContext.GetCurrentID();
                  gContext.mBoundsMatrix = gContext.mModelSource;
              }
        }

        if (gContext.mbUsingBounds && (gContext.GetCurrentID() == gContext.mEditingID))
        {
            glm::mat4 scale = glm::mat4(1.0f);

            const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mBoundsPlan);
            glm::vec4 newPos = gContext.mRayOrigin + gContext.mRayVector * len;

            glm::vec4 deltaVector = glm::abs(newPos - gContext.mBoundsPivot);
            glm::vec4 referenceVector = glm::abs(gContext.mBoundsAnchor - gContext.mBoundsPivot);

            for (int i = 0; i < 2; i++)
            {
                int axisIndex1 = gContext.mBoundsAxis[i];
                if (axisIndex1 == -1)
                {
                    continue;
                }

                float ratioAxis = 1.f;
                glm::vec4 axisDir = glm::abs(gContext.mBoundsMatrix[axisIndex1]);

                float dtAxis = glm::dot(axisDir, referenceVector);
                float boundSize = bounds[axisIndex1 + 3] - bounds[axisIndex1];
                if (dtAxis > FLT_EPSILON)
                {
                    ratioAxis = glm::dot(axisDir, deltaVector) / dtAxis;
                }

                if (snapValues)
                {
                    float length = boundSize * ratioAxis;
                    ComputeSnap(&length, snapValues[axisIndex1]);
                    if (boundSize > FLT_EPSILON)
                    {
                        ratioAxis = length / boundSize;
                    }
                }
                scale[axisIndex1] *= ratioAxis;

                if (fabsf(ratioAxis - 1.0f) > FLT_EPSILON) {
                    manipulated = true;
                }
            }

            glm::mat4 preScale = glm::translate(glm::mat4(1.0f), -glm::vec3(gContext.mBoundsLocalPivot));
            glm::mat4 postScale = glm::translate(glm::mat4(1.0f), glm::vec3(gContext.mBoundsLocalPivot));
            glm::mat4 res = preScale * scale * postScale * gContext.mBoundsMatrix;
            *matrix = res;

            char tmps[512];
            glm::vec2 destinationPosOnScreen = worldToPos(glm::vec4(glm::vec3(gContext.mModel[3]), 1.f), gContext.mViewProjection, vpPos, vpSize);
            FormatString(tmps, sizeof(tmps), "X: %.2f Y: %.2f Z: %.2f"
                , (bounds[3] - bounds[0]) * glm::length(glm::vec3(gContext.mBoundsMatrix[0])) * glm::length(glm::vec3(scale[0]))
                , (bounds[4] - bounds[1]) * glm::length(glm::vec3(gContext.mBoundsMatrix[1])) * glm::length(glm::vec3(scale[1]))
                , (bounds[5] - bounds[2]) * glm::length(glm::vec3(gContext.mBoundsMatrix[2])) * glm::length(glm::vec3(scale[2]))
            );
            drawList->AddText(glm::vec2(destinationPosOnScreen.x + 15, destinationPosOnScreen.y + 15), GetColorU32(TEXT_SHADOW), tmps);
            drawList->AddText(glm::vec2(destinationPosOnScreen.x + 14, destinationPosOnScreen.y + 14), GetColorU32(TEXT), tmps);
        }

        if (!gContext.mInput->getMouseButton(MouseButton::Left))
        {
            gContext.mbUsingBounds = false;
            gContext.mEditingID = -1;
        }

        if (gContext.mbUsingBounds)
        {
            break;
        }
     }

     return manipulated;
  }


  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //

  static MOVETYPE GetScaleType(OPERATION op)
  {
      if (gContext.mbUsing)
      {
          return MT_NONE;
      }
      IInput* input = gContext.mInput;
      if (!input) return MT_NONE;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      glm::vec2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
      MOVETYPE type = MT_NONE;

      if (static_cast<float>(mouseX) >= gContext.mScreenSquareMin.x && static_cast<float>(mouseX) <= gContext.mScreenSquareMax.x &&
          static_cast<float>(mouseY) >= gContext.mScreenSquareMin.y && static_cast<float>(mouseY) <= gContext.mScreenSquareMax.y &&
          Contains(op, SCALE))
      {
          type = MT_SCALE_XYZ;
      }

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < 3 && type == MT_NONE; i++)
      {
          if (!Intersects(op, static_cast<OPERATION>(SCALE_X << i)))
          {
              continue;
          }
          bool isAxisMasked = ((1 << i) & gContext.mAxisMask) != 0;

          glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
          bool belowAxisLit, belowPlaneLit;
          ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit, true);

          dirAxis = gContext.mModelLocal * glm::vec4(glm::vec3(dirAxis), 0.0f);
          dirPlaneX = gContext.mModelLocal * glm::vec4(glm::vec3(dirPlaneX), 0.0f);
          dirPlaneY = gContext.mModelLocal * glm::vec4(glm::vec3(dirPlaneY), 0.0f);

          const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, BuildPlan(glm::vec4(glm::vec3(gContext.mModelLocal[3]), 1.f), dirAxis));
          glm::vec4 posOnPlan = gContext.mRayOrigin + gContext.mRayVector * len;

          const float startOffset = Contains(op, static_cast<OPERATION>(TRANSLATE_X << i)) ? 1.0f : 0.1f;
          const float endOffset = Contains(op, static_cast<OPERATION>(TRANSLATE_X << i)) ? 1.4f : 1.0f;

          const glm::vec2 posOnPlanScreen = worldToPos(posOnPlan, gContext.mViewProjection, vpPos, vpSize);
          const glm::vec2 axisStartOnScreen = worldToPos(glm::vec4(glm::vec3(gContext.mModelLocal[3]), 1.f) + dirAxis * gContext.mScreenFactor * startOffset, gContext.mViewProjection, vpPos, vpSize);
          const glm::vec2 axisEndOnScreen = worldToPos(glm::vec4(glm::vec3(gContext.mModelLocal[3]), 1.f) + dirAxis * gContext.mScreenFactor * endOffset, gContext.mViewProjection, vpPos, vpSize);

          glm::vec4 closestPointOnAxis = PointOnSegment(glm::vec4(posOnPlanScreen, 0.f, 0.f), glm::vec4(axisStartOnScreen, 0.f, 0.f), glm::vec4(axisEndOnScreen, 0.f, 0.f));

          if (glm::distance(glm::vec2(closestPointOnAxis.x, closestPointOnAxis.y), posOnPlanScreen) < 12.f)
          {
              if (!isAxisMasked)
                  type = static_cast<MOVETYPE>(MT_SCALE_X + i);
          }
      }

      glm::vec2 deltaScreen = glm::vec2(
          static_cast<float>(mouseX - gContext.mScreenSquareCenter.x),
          static_cast<float>(mouseY - gContext.mScreenSquareCenter.y)
      );

      float dist = glm::length(deltaScreen);
      if (Contains(op, SCALEU) && dist >= 17.0f && dist < 23.0f)
      {
          type = MT_SCALE_XYZ;
      }

      for (int i = 0; i < 3 && type == MT_NONE; i++)
      {
          if (!Intersects(op, static_cast<OPERATION>(SCALE_XU << i)))
          {
              continue;
          }

          glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
          bool belowAxisLit, belowPlaneLit;
          ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit, true);

          if (belowAxisLit)
          {
              bool hasTranslateOnAxis = Contains(op, static_cast<OPERATION>(TRANSLATE_X << i));
              float markerScale = hasTranslateOnAxis ? 1.4f : 1.0f;
              glm::vec2 baseSSpace = worldToPos(dirAxis * 0.1f * gContext.mScreenFactor, gContext.mMVPLocal, vpPos, vpSize);
              glm::vec2 worldDirSSpaceNoScale = worldToPos(dirAxis * markerScale * gContext.mScreenFactor, gContext.mMVP, vpPos, vpSize);
              glm::vec2 worldDirSSpace = worldToPos((dirAxis * markerScale) * gContext.mScreenFactor, gContext.mMVPLocal, vpPos, vpSize);

              float distance = glm::distance(worldDirSSpace, mousePos);
              if (distance < 12.f)
              {
                  type = static_cast<MOVETYPE>(MT_SCALE_X + i);
              }
          }
      }
      return type;
  }

  static MOVETYPE GetRotateType(OPERATION op)
  {
      IInput* input = gContext.mInput;
      if (!input) return MT_NONE;

      bool isNoAxesMasked = !gContext.mAxisMask;
      bool isMultipleAxesMasked = (gContext.mAxisMask & (gContext.mAxisMask - 1)) != 0;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      glm::vec2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));
      MOVETYPE type = MT_NONE;

      glm::vec2 deltaScreen = glm::vec2(
          static_cast<float>(mouseX - gContext.mScreenSquareCenter.x),
          static_cast<float>(mouseY - gContext.mScreenSquareCenter.y)
      );
      float dist = glm::length(deltaScreen);
      if (Intersects(op, ROTATE_SCREEN) && dist >= (gContext.mRadiusSquareCenter - 4.0f) && dist < (gContext.mRadiusSquareCenter + 4.0f))
      {
          if (!isNoAxesMasked)
              return MT_NONE;
          type = MT_ROTATE_SCREEN;
      }

      glm::mat4 modelMat = gContext.mModel;
      glm::mat4 viewMat = gContext.mViewMat;
      glm::mat4 modelInverseMat = gContext.mModelInverse;
      glm::mat4 mvpMat = gContext.mMVP;

      glm::vec4 modelPos = glm::vec4(glm::vec3(modelMat[3]), 1.0f);
      glm::vec4 planNormals[] = { modelMat[0], modelMat[1], modelMat[2] };

      glm::vec4 modelViewPos = viewMat * modelPos;

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < 3 && type == MT_NONE; i++)
      {
          if (!Intersects(op, static_cast<OPERATION>(ROTATE_X << i)))
          {
              continue;
          }
          bool isAxisMasked = ((1 << i) & gContext.mAxisMask) != 0;

          glm::vec4 pickupPlan = BuildPlan(modelPos, planNormals[i]);

          const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, pickupPlan);
          const glm::vec4 intersectWorldPos = gContext.mRayOrigin + gContext.mRayVector * len;

          glm::vec4 intersectViewPos = viewMat * glm::vec4(glm::vec3(intersectWorldPos), 1.0f);

          if (glm::abs(modelViewPos.z) - glm::abs(intersectViewPos.z) < -FLT_EPSILON)
          {
              continue;
          }

          const glm::vec4 localPos = intersectWorldPos - modelPos;
          glm::vec3 normLocal = glm::vec3(localPos);
          if (glm::length(normLocal) > FLT_EPSILON)
          {
              normLocal = glm::normalize(normLocal);
          }

          glm::vec4 idealPosOnCircle = modelInverseMat * glm::vec4(normLocal, 0.0f);

          glm::vec4 idealPosWorld = glm::vec4(glm::vec3(idealPosOnCircle) * (rotationDisplayFactor * gContext.mScreenFactor), 1.0f);
          const glm::vec2 idealPosOnCircleScreen = worldToPos(idealPosWorld, mvpMat, vpPos, vpSize);

          gContext.mDrawList->AddCircle(idealPosOnCircleScreen, 5.f, 0xFFFFFFFF);
          const glm::vec2 distanceOnScreen = idealPosOnCircleScreen - mousePos;

          const float distance = glm::length(distanceOnScreen);
          if (distance < 8.f)
          {
              if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked)
                  break;
              type = static_cast<MOVETYPE>(MT_ROTATE_X + i);
          }
      }

      return type;
  }

  static MOVETYPE GetMoveType(OPERATION op, glm::vec4* gizmoHitProportion)
  {
      if (!Intersects(op, TRANSLATE) || gContext.mbUsing || !gContext.mbMouseOver)
      {
          return MT_NONE;
      }

      bool isNoAxesMasked = !gContext.mAxisMask;
      bool isMultipleAxesMasked = (gContext.mAxisMask & (gContext.mAxisMask - 1)) != 0;

      IInput* input = gContext.mInput;
      if (!input) return MT_NONE;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      MOVETYPE type = MT_NONE;

      if (static_cast<float>(mouseX) >= gContext.mScreenSquareMin.x && static_cast<float>(mouseX) <= gContext.mScreenSquareMax.x &&
          static_cast<float>(mouseY) >= gContext.mScreenSquareMin.y && static_cast<float>(mouseY) <= gContext.mScreenSquareMax.y &&
          Contains(op, TRANSLATE))
      {
          type = MT_MOVE_SCREEN;
      }

      glm::vec2 mouseLocal = glm::vec2(static_cast<float>(mouseX), static_cast<float>(mouseY)) - glm::vec2(gContext.mX, gContext.mY);
      glm::vec4 screenCoord = glm::vec4(mouseLocal, 0.f, 0.f);

      glm::mat4 modelMat = gContext.mModel;
      glm::mat4 viewProjMat = gContext.mViewProjection;
      glm::vec4 modelPos = glm::vec4(glm::vec3(modelMat[3]), 1.0f);

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < 3 && type == MT_NONE; i++)
      {
          bool isAxisMasked = ((1 << i) & gContext.mAxisMask) != 0;
          glm::vec4 dirPlaneX, dirPlaneY, dirAxis;
          bool belowAxisLit, belowPlaneLit;
          ComputeTripodAxisAndVisibility(i, dirAxis, dirPlaneX, dirPlaneY, belowAxisLit, belowPlaneLit);

          dirAxis = modelMat * glm::vec4(glm::vec3(dirAxis), 0.f);
          dirPlaneX = modelMat * glm::vec4(glm::vec3(dirPlaneX), 0.f);
          dirPlaneY = modelMat * glm::vec4(glm::vec3(dirPlaneY), 0.f);

          const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, BuildPlan(modelPos, dirAxis));
          glm::vec4 posOnPlan = gContext.mRayOrigin + gContext.mRayVector * len;

          const glm::vec2 axisStartOnScreen = worldToPos(modelPos + dirAxis * gContext.mScreenFactor * 0.1f, viewProjMat, vpPos, vpSize) - glm::vec2(gContext.mX, gContext.mY);
          const glm::vec2 axisEndOnScreen = worldToPos(modelPos + dirAxis * gContext.mScreenFactor, viewProjMat, vpPos, vpSize) - glm::vec2(gContext.mX, gContext.mY);

          glm::vec4 closestPointOnAxis = PointOnSegment(screenCoord, glm::vec4(axisStartOnScreen, 0.f, 0.f), glm::vec4(axisEndOnScreen, 0.f, 0.f));
          if (glm::length(glm::vec3(closestPointOnAxis - screenCoord)) < 12.f && Intersects(op, static_cast<OPERATION>(TRANSLATE_X << i)))
          {
              if (isAxisMasked)
                  break;
              type = static_cast<MOVETYPE>(MT_MOVE_X + i);
          }

          const float dx = glm::dot(glm::vec3(dirPlaneX), glm::vec3(posOnPlan - modelPos) * (1.f / gContext.mScreenFactor));
          const float dy = glm::dot(glm::vec3(dirPlaneY), glm::vec3(posOnPlan - modelPos) * (1.f / gContext.mScreenFactor));
          if (belowPlaneLit && dx >= quadUV[0] && dx <= quadUV[4] && dy >= quadUV[1] && dy <= quadUV[3] && Contains(op, TRANSLATE_PLANS[i]))
          {
              if ((!isAxisMasked || isMultipleAxesMasked) && !isNoAxesMasked)
                  break;
              type = static_cast<MOVETYPE>(MT_MOVE_YZ + i);
          }

          if (gizmoHitProportion)
          {
              *gizmoHitProportion = glm::vec4(dx, dy, 0.f, 0.f);
          }
      }
      return type;
  }

  static bool HandleTranslation(float* matrix, float* deltaMatrix, OPERATION op, MOVETYPE& type, const float* snap)
  {
	  std::cout << "HandleTranslation called with op=" << op << " type=" << type << "\n";
      if (!Intersects(op, TRANSLATE) || type != MT_NONE)
      {
          return false;
      }

      IInput* input = gContext.mInput;
      if (!input) return false;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      const bool applyRotationLocaly = gContext.mMode == LOCAL || gContext.mCurrentHandleType == MT_MOVE_SCREEN;
      bool modified = false;

      glm::mat4 modelMat = gContext.mModel;
      glm::vec4 modelPos = glm::vec4(glm::vec3(modelMat[3]), 1.0f);

      if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsTranslateType(gContext.mCurrentHandleType))
      {
          const float signedLength = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
          const float len = fabsf(signedLength);
          const glm::vec4 newPos = gContext.mRayOrigin + gContext.mRayVector * len;

          const glm::vec4 newOrigin = newPos - gContext.mRelativeOrigin * gContext.mScreenFactor;
          glm::vec4 delta = newOrigin - modelPos;

          if (gContext.mCurrentHandleType >= MT_MOVE_X && gContext.mCurrentHandleType <= MT_MOVE_Z)
          {
              const int axisIndex = gContext.mCurrentHandleType - MT_MOVE_X;
              const glm::vec4 axisValue = glm::vec4(glm::normalize(glm::vec3(modelMat[axisIndex])), 0.0f);
              const float lengthOnAxis = glm::dot(axisValue, delta);
              delta = axisValue * lengthOnAxis;
          }

          if (snap)
          {
              glm::vec4 cumulativeDelta = modelPos + delta - gContext.mMatrixOrigin;
              if (applyRotationLocaly)
              {
                  glm::mat4 modelSourceNormalized = gContext.mModelSource;
                  glm::vec3 r = glm::normalize(glm::vec3(modelSourceNormalized[0]));
                  glm::vec3 u = glm::normalize(glm::vec3(modelSourceNormalized[1]) - glm::dot(r, glm::vec3(modelSourceNormalized[1])) * r);
                  glm::vec3 d = glm::cross(r, u);
                  modelSourceNormalized[0] = glm::vec4(r, 0.0f);
                  modelSourceNormalized[1] = glm::vec4(u, 0.0f);
                  modelSourceNormalized[2] = glm::vec4(d, 0.0f);

                  glm::mat4 modelSourceNormalizedInverse = glm::inverse(modelSourceNormalized);
                  cumulativeDelta = modelSourceNormalizedInverse * glm::vec4(glm::vec3(cumulativeDelta), 0.0f);
                  ComputeSnap(&cumulativeDelta.x, snap[0]); // Пример вызова, адаптируйте под вашу ComputeSnap
                  cumulativeDelta = modelSourceNormalized * glm::vec4(glm::vec3(cumulativeDelta), 0.0f);
              }
              else
              {
                  ComputeSnap(&cumulativeDelta.x, snap[0]);
              }
              delta = gContext.mMatrixOrigin + cumulativeDelta - modelPos;
          }

          if (delta != gContext.mTranslationLastDelta)
          {
              modified = true;
          }
          gContext.mTranslationLastDelta = delta;

          glm::mat4 deltaMatrixTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(delta));
          if (deltaMatrix)
          {
              memcpy(deltaMatrix, glm::value_ptr(deltaMatrixTranslation), sizeof(float) * 16);
          }

          const glm::mat4 res = gContext.mModelSource * deltaMatrixTranslation;
          *(glm::mat4*)matrix = res;

          if (!input->getMouseButton(MouseButton::Left))
          {
              gContext.mbUsing = false;
          }

          type = gContext.mCurrentHandleType;
      }
      else
      {
          glm::vec4 gizmoHitProportion;
          type = gContext.mbOverGizmoHotspot ? MT_NONE : GetMoveType(op, &gizmoHitProportion);
          gContext.mbOverGizmoHotspot |= type != MT_NONE;

          std::cout
              << "CanActivate=" << CanActivate()
              << " type=" << type
              << "\n";

          if (CanActivate() && type != MT_NONE)
          {
              gContext.mbUsing = true;
              gContext.mEditingID = gContext.GetCurrentID();
              gContext.mCurrentHandleType = type;

              glm::vec4 movePlanNormal[] = {
                 glm::vec4(glm::vec3(modelMat[0]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[1]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[2]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[0]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[1]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[2]), 0.0f),
                 -gContext.mCameraDir
              };

              glm::vec3 camToModel = glm::vec3(modelPos) - glm::vec3(gContext.mCameraEye);
              if (glm::length(camToModel) > FLT_EPSILON)
              {
                  camToModel = glm::normalize(camToModel);
              }
              glm::vec4 cameraToModelNormalized = glm::vec4(camToModel, 0.0f);

              for (unsigned int i = 0; i < 3; i++)
              {
                  glm::vec3 orthoVector = glm::cross(glm::vec3(movePlanNormal[i]), glm::vec3(cameraToModelNormalized));
                  glm::vec3 planNorm = glm::cross(orthoVector, glm::vec3(movePlanNormal[i]));
                  if (glm::length(planNorm) > FLT_EPSILON)
                  {
                      planNorm = glm::normalize(planNorm);
                  }
                  movePlanNormal[i] = glm::vec4(planNorm, 0.0f);
              }

              gContext.mTranslationPlan = BuildPlan(modelPos, movePlanNormal[type - MT_MOVE_X]);
              const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
              gContext.mTranslationPlanOrigin = gContext.mRayOrigin + gContext.mRayVector * len;
              gContext.mMatrixOrigin = modelPos;

              gContext.mRelativeOrigin = (gContext.mTranslationPlanOrigin - modelPos) * (1.f / gContext.mScreenFactor);
          }
      }
      return modified;
  }

  static bool HandleScale(float* matrix, float* deltaMatrix, OPERATION op, MOVETYPE& type, const float* snap)
  {
      if ((!Intersects(op, SCALE) && !Intersects(op, SCALEU)) || type != MT_NONE || !gContext.mbMouseOver)
      {
          return false;
      }

      IInput* input = gContext.mInput;
      if (!input) return false;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      bool modified = false;

      glm::mat4 modelLocalMat = gContext.mModelLocal;
      glm::vec4 modelLocalPos = glm::vec4(glm::vec3(modelLocalMat[3]), 1.0f);
      glm::mat4 modelSourceMat = gContext.mModelSource;

      if (!gContext.mbUsing)
      {
          type = gContext.mbOverGizmoHotspot ? MT_NONE : GetScaleType(op);
          gContext.mbOverGizmoHotspot |= type != MT_NONE;

          if (CanActivate() && type != MT_NONE)
          {
              gContext.mbUsing = true;
              gContext.mEditingID = gContext.GetCurrentID();
              gContext.mCurrentHandleType = type;

              glm::vec4 movePlanNormal[] = {
                 glm::vec4(glm::vec3(modelLocalMat[0]), 0.0f), // Ось X
                 glm::vec4(glm::vec3(modelLocalMat[1]), 0.0f), // Ось Y
                 glm::vec4(glm::vec3(modelLocalMat[2]), 0.0f), // Ось Z
                 glm::vec4(glm::vec3(modelLocalMat[0]), 0.0f), // Дополнительные плоскости, если они нужны для SCALE
                 glm::vec4(glm::vec3(modelLocalMat[1]), 0.0f),
                 glm::vec4(glm::vec3(modelLocalMat[2]), 0.0f), // Опечатка с ] исправлена здесь
                 -gContext.mCameraDir
              };

              gContext.mTranslationPlan = BuildPlan(modelLocalPos, movePlanNormal[type - MT_SCALE_X]);
              const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
              gContext.mTranslationPlanOrigin = gContext.mRayOrigin + gContext.mRayVector * len;
              gContext.mMatrixOrigin = modelLocalPos;
              gContext.mScale = glm::vec4(1.f, 1.f, 1.f, 0.f);
              gContext.mRelativeOrigin = (gContext.mTranslationPlanOrigin - modelLocalPos) * (1.f / gContext.mScreenFactor);

              gContext.mScaleValueOrigin = glm::vec4(
                  glm::length(glm::vec3(modelSourceMat[0])), // Длина вектора X
                  glm::length(glm::vec3(modelSourceMat[1])), // Длина вектора Y
                  glm::length(glm::vec3(modelSourceMat[2])), // Длина вектора Z
                  0.f
              );
              gContext.mSaveMousePosx = static_cast<float>(mouseX);
          }
      }


      if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsScaleType(gContext.mCurrentHandleType))
      {
          const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
          glm::vec4 newPos = gContext.mRayOrigin + gContext.mRayVector * len;
          glm::vec4 newOrigin = newPos - gContext.mRelativeOrigin * gContext.mScreenFactor;
          glm::vec4 delta = newOrigin - modelLocalPos;

          if (gContext.mCurrentHandleType >= MT_SCALE_X && gContext.mCurrentHandleType <= MT_SCALE_Z)
          {
              int axisIndex = gContext.mCurrentHandleType - MT_SCALE_X;
              glm::vec4 axisValue = glm::vec4(glm::normalize(glm::vec3(modelLocalMat[axisIndex])), 0.0f);
              float lengthOnAxis = glm::dot(axisValue, delta);
              delta = axisValue * lengthOnAxis;

              glm::vec4 baseVector = gContext.mTranslationPlanOrigin - modelLocalPos;
              float ratio = glm::dot(axisValue, baseVector + delta) / glm::dot(axisValue, baseVector);

              gContext.mScale[axisIndex] = glm::max(ratio, 0.001f);
          }
          else
          {
              float scaleDelta = (static_cast<float>(mouseX) - gContext.mSaveMousePosx) * 0.01f;
              gContext.mScale = glm::vec4(glm::max(1.f + scaleDelta, 0.001f));
          }

          if (snap)
          {
              ComputeSnap(&gContext.mScale.x, snap[0]);
          }


          for (int i = 0; i < 3; i++)
              gContext.mScale[i] = glm::max(gContext.mScale[i], 0.001f);

          if (gContext.mScaleLast != gContext.mScale)
          {
              modified = true;
          }
          gContext.mScaleLast = gContext.mScale;

          glm::mat4 deltaMatrixScale = glm::scale(glm::mat4(1.0f), glm::vec3(gContext.mScale * gContext.mScaleValueOrigin));

          glm::mat4 res = deltaMatrixScale * modelLocalMat;
          *(glm::mat4*)matrix = res;

          if (deltaMatrix)
          {
              glm::vec4 deltaScale = gContext.mScale * gContext.mScaleValueOrigin;
              glm::vec4 originalScaleDivider = glm::vec4(
                  1.f / gContext.mModelScaleOrigin.x,
                  1.f / gContext.mModelScaleOrigin.y,
                  1.f / gContext.mModelScaleOrigin.z,
                  0.f
              );

              deltaScale = deltaScale * originalScaleDivider;
              deltaMatrixScale = glm::scale(glm::mat4(1.0f), glm::vec3(deltaScale));
              memcpy(deltaMatrix, glm::value_ptr(deltaMatrixScale), sizeof(float) * 16);
          }

          if (!input->getMouseButton(MouseButton::Left))
          {
              gContext.mbUsing = false;
              gContext.mScale = glm::vec4(1.f, 1.f, 1.f, 0.f);
          }

          type = gContext.mCurrentHandleType;
      }
      return modified;
  }

  static bool HandleRotation(float* matrix, float* deltaMatrix, OPERATION op, MOVETYPE& type, const float* snap)
  {
      if (!Intersects(op, ROTATE) || type != MT_NONE || !gContext.mbMouseOver)
      {
          return false;
      }

      IInput* input = gContext.mInput;
      if (!input) return false;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      bool applyRotationLocaly = gContext.mMode == LOCAL;
      bool modified = false;

      glm::mat4 modelMat = gContext.mModel;
      glm::mat4 modelSourceMat = gContext.mModelSource;

      glm::vec4 modelPos = glm::vec4(glm::vec3(modelMat[3]), 1.0f);
      glm::vec4 modelSourcePos = glm::vec4(glm::vec3(modelSourceMat[3]), 1.0f);

      if (!gContext.mbUsing)
      {
          int type = gContext.mbOverGizmoHotspot ? MT_NONE : GetRotateType(op);
          gContext.mbOverGizmoHotspot |= type != MT_NONE;

          if (type == MT_ROTATE_SCREEN)
          {
              applyRotationLocaly = true;
          }

          if (CanActivate() && type != MT_NONE)
          {
              gContext.mbUsing = true;
              gContext.mEditingID = gContext.GetCurrentID();
              gContext.mCurrentHandleType = static_cast<MOVETYPE>(type);

              glm::vec4 rotatePlanNormal[] = {
                 glm::vec4(glm::vec3(modelMat[0]), 0.0f),
                 glm::vec4(glm::vec3(modelMat[1]), 0.0f), // Ось Y
                 glm::vec4(glm::vec3(modelMat[2]), 0.0f), // Ось Z
                 -gContext.mCameraDir
              };


              if (applyRotationLocaly)
              {
                  gContext.mTranslationPlan = BuildPlan(modelPos, rotatePlanNormal[type - MT_ROTATE_X]);
              }
              else
              {
                  gContext.mTranslationPlan = BuildPlan(modelSourcePos, directionUnary[type - MT_ROTATE_X]);
              }

              const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, gContext.mTranslationPlan);
              glm::vec3 localPos3 = glm::vec3(gContext.mRayOrigin + gContext.mRayVector * len - modelPos);
              if (glm::length(localPos3) > FLT_EPSILON)
              {
                  localPos3 = glm::normalize(localPos3);
              }
              gContext.mRotationVectorSource = glm::vec4(localPos3, 0.0f);
              gContext.mRotationAngleOrigin = ComputeAngleOnPlan();
          }
      }

      if (gContext.mbUsing && (gContext.GetCurrentID() == gContext.mEditingID) && IsRotateType(gContext.mCurrentHandleType))
      {
          gContext.mRotationAngle = ComputeAngleOnPlan();
          if (snap)
          {
              float snapInRadian = snap[0] * DEG2RAD;
              ComputeSnap(&gContext.mRotationAngle, snapInRadian);
          }

          glm::vec4 rotationAxisLocalSpace = gContext.mModelInverse * glm::vec4(glm::vec3(gContext.mTranslationPlan), 0.0f);
          glm::vec3 normAxis = glm::vec3(rotationAxisLocalSpace);
          if (glm::length(normAxis) > FLT_EPSILON)
          {
              normAxis = glm::normalize(normAxis);
          }
          rotationAxisLocalSpace = glm::vec4(normAxis, 0.0f);

          glm::mat4 deltaRotation = glm::rotate(glm::mat4(1.0f), gContext.mRotationAngle - gContext.mRotationAngleOrigin, glm::vec3(rotationAxisLocalSpace));
          if (gContext.mRotationAngle != gContext.mRotationAngleOrigin)
          {
              modified = true;
          }
          gContext.mRotationAngleOrigin = gContext.mRotationAngle;

          glm::mat4 scaleOrigin = glm::scale(glm::mat4(1.0f), glm::vec3(gContext.mModelScaleOrigin));

          if (applyRotationLocaly)
          {
              *(glm::mat4*)matrix = scaleOrigin * deltaRotation * gContext.mModelLocal;
          }
          else
          {
              glm::mat4 res = modelSourceMat;
              res[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

              glm::mat4 finalMat = res * deltaRotation;
              finalMat[3] = modelSourceMat[3];
              *(glm::mat4*)matrix = finalMat;
          }

          if (deltaMatrix)
          {
              *(glm::mat4*)deltaMatrix = gContext.mModelInverse * deltaRotation * modelMat;
          }

          if (!input->getMouseButton(MouseButton::Left))
          {
              gContext.mbUsing = false;
              gContext.mEditingID = -1;
          }
          type = gContext.mCurrentHandleType;
      }
      return modified;
  }

  void DecomposeMatrixToComponents(const float* matrix, float* translation, float* rotation, float* scale)
  {
      glm::mat4 mat = *(const glm::mat4*)matrix;

      scale[0] = glm::length(glm::vec3(mat[0]));
      scale[1] = glm::length(glm::vec3(mat[1]));
      scale[2] = glm::length(glm::vec3(mat[2]));

      glm::vec3 r = glm::normalize(glm::vec3(mat[0]));
      glm::vec3 u = glm::normalize(glm::vec3(mat[1]) - glm::dot(r, glm::vec3(mat[1])) * r);
      glm::vec3 d = glm::cross(r, u);

      mat[0] = glm::vec4(r, 0.0f);
      mat[1] = glm::vec4(u, 0.0f);
      mat[2] = glm::vec4(d, 0.0f);

      rotation[0] = RAD2DEG * atan2f(mat[1][2], mat[2][2]);
      rotation[1] = RAD2DEG * atan2f(-mat[0][2], sqrtf(mat[1][2] * mat[1][2] + mat[2][2] * mat[2][2]));
      rotation[2] = RAD2DEG * atan2f(mat[0][1], mat[0][0]);

      translation[0] = mat[3].x;
      translation[1] = mat[3].y;
      translation[2] = mat[3].z;
  }

  void RecomposeMatrixFromComponents(const float* translation, const float* rotation, const float* scale, float* matrix)
  {
      glm::mat4& mat = *(glm::mat4*)matrix;

      glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation[0] * DEG2RAD, glm::vec3(1.0f, 0.0f, 0.0f));
      glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation[1] * DEG2RAD, glm::vec3(0.0f, 1.0f, 0.0f));
      glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation[2] * DEG2RAD, glm::vec3(0.0f, 0.0f, 1.0f));

      mat = rotX * rotY * rotZ;

      float validScale[3];
      for (int i = 0; i < 3; i++)
      {
          if (fabsf(scale[i]) < FLT_EPSILON)
          {
              validScale[i] = 0.001f;
          }
          else
          {
              validScale[i] = scale[i];
          }
      }

      mat[0] *= validScale[0];
      mat[1] *= validScale[1];
      mat[2] *= validScale[2];
      mat[3] = glm::vec4(translation[0], translation[1], translation[2], 1.0f);
  }

  glm::vec4 BuildPlan(const glm::vec4& point, const glm::vec4& normal)
  {
      glm::vec3 n = glm::normalize(glm::vec3(normal));
      float d = -glm::dot(n, glm::vec3(point));
      return glm::vec4(n, d);
  }

  void SetAlternativeWindow(GuiWindow* window)
  {
     gContext.mAlternativeWindow = window;
  }

  void SetID(int id)
  {
     if (gContext.mIDStack.empty())
     {
        gContext.mIDStack.push_back(-1);
     }
     gContext.mIDStack.back() = id;
  }

  WidgetID GetID(const char* str, const char* str_end)
  {
     WidgetID seed = gContext.GetCurrentID();
     WidgetID id = HashStr(str, str_end ? (str_end - str) : 0, seed);
     return id;
  }

  WidgetID GetID(const char* str)
  {
     return GetID(str, nullptr);
  }

  WidgetID GetID(const void* ptr)
  {
     WidgetID seed = gContext.GetCurrentID();
     WidgetID id = HashData(&ptr, sizeof(void*), seed);
     return id;
  }

  WidgetID GetID(int n)
  {
     WidgetID seed = gContext.GetCurrentID();
     WidgetID id = HashData(&n, sizeof(n), seed);
     return id;
  }

  void PushID(const char* str_id)
  {
     WidgetID id = GetID(str_id);
     gContext.mIDStack.push_back(id);
  }

  void PushID(const char* str_id_begin, const char* str_id_end)
  {
     WidgetID id = GetID(str_id_begin, str_id_end);
     gContext.mIDStack.push_back(id);
  }

  void PushID(const void* ptr_id)
  {
     WidgetID id = GetID(ptr_id);
     gContext.mIDStack.push_back(id);
  }

  void PushID(int int_id)
  {
     WidgetID id = GetID(int_id);
     gContext.mIDStack.push_back(id);
  }

  void PopID()
  {
     _ASSERT(gContext.mIDStack.size() > 1);
     gContext.mIDStack.pop_back();
     if (gContext.mIDStack.empty())
     {
        gContext.mIDStack.clear();
     }
  }

  void AllowAxisFlip(bool value)
  {
    gContext.mAllowAxisFlip = value;
  }

  void SetAxisLit(float value)
  {
    gContext.mAxisLit=value;
  }

  void SetAxisMask(bool x, bool y, bool z)
  {
     gContext.mAxisMask = (x ? 1 : 0) + (y ? 2 : 0) + (z ? 4 : 0);
  }

  void SetPlaneLit(float value)
  {
    gContext.mPlaneLit = value;
  }

  bool IsOver(float* position, float pixelRadius)
  {
      if (!gContext.mbEnable || !gContext.mInput)
      {
          return false;
      }

      double mouseX, mouseY;
      gContext.mInput->getMousePosition(mouseX, mouseY);
      glm::vec2 mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));

      glm::vec4 worldPos = makeVect(position[0], position[1], position[2], 1.0f);

      glm::vec2 viewportPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 viewportSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      glm::vec2 screenPos = worldToPos(worldPos, gContext.mMVP, viewportPos, viewportSize);

      glm::vec2 delta = mousePos - screenPos;
      float distanceSqr = delta.x * delta.x + delta.y * delta.y;

      return distanceSqr <= (pixelRadius * pixelRadius);
  }

  Ray ComputeCameraRay()
  {
      glm::vec2 position =
      { gContext.mX, gContext.mY };
      glm::vec2 size =
      { gContext.mWidth, gContext.mHeight };
      ComputeCameraRay(gContext.mRayOrigin, gContext.mRayVector, position, size);
      Ray ray;

      ray.origin = gContext.mRayOrigin;
      ray.direction = gContext.mRayVector;

      return ray;
  }

  bool Manipulate(const float* view, const float* projection, OPERATION operation, MODE mode, float* matrix, float* deltaMatrix, const float* snap, const float* localBounds, const float* boundsSnap)
  {
      gContext.mDrawList->PushClipRect(glm::vec2(gContext.mX, gContext.mY), glm::vec2(gContext.mX + gContext.mWidth, gContext.mY + gContext.mHeight), false);

      ComputeContext(view, projection, matrix, (operation & SCALE) ? LOCAL : mode);
      gContext.mHoveredHandleType = MT_NONE;

      if (deltaMatrix)
      {
          *(glm::mat4*)deltaMatrix = glm::mat4(1.0f);
      }

      glm::mat4 mvpMat = gContext.mMVP;
      glm::vec4 camSpacePosition = mvpMat * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
      glm::vec4 viewPos = gContext.mViewMat * gContext.mModel[3];
      std::cout
          << "viewPos = "
          << viewPos.x << " "
          << viewPos.y << " "
          << viewPos.z << " "
          << viewPos.w << "\n";
      //if (!gContext.mIsOrthographic &&
      //    viewPos.z > -0.001f &&
      //    !gContext.mbUsing)
      //{
      //    gContext.mDrawList->PopClipRect();
      //    return false;
      //}

      MOVETYPE type = MT_NONE;
      //bool manipulated = false;
      //if (gContext.mbEnable)
      //{
      //    if (!gContext.mbUsingBounds)
      //    {
      //        manipulated = HandleTranslation(matrix, deltaMatrix, operation, type, snap) ||
      //            HandleScale(matrix, deltaMatrix, operation, type, snap) ||
      //            HandleRotation(matrix, deltaMatrix, operation, type, snap);
      //    }
      //}

      //if (localBounds && !gContext.mbUsing)
      //{
      //    manipulated |= HandleAndDrawLocalBounds(localBounds, (glm::mat4*)matrix, boundsSnap, operation);
      //}

      gContext.mOperation = operation;
      gContext.mHoveredHandleType = (!gContext.mbUsing && !gContext.mbUsingBounds) ? type : MT_NONE;
      if (!gContext.mbUsingBounds)
      {
          DrawRotationGizmo(operation, type);
          DrawTranslationGizmo(operation, type);
          DrawScaleGizmo(operation, type);
          DrawScaleUniveralGizmo(operation, type);
      }

      gContext.mDrawList->PopClipRect();
      return 1;
  }

  void SetGizmoSizeClipSpace(float value)
  {
     gContext.mGizmoSizeClipSpace = value;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////////
  void ComputeFrustumPlanes(glm::vec4* frustum, const float* clip)
  {
     frustum[0].x = clip[3] - clip[0];
     frustum[0].y = clip[7] - clip[4];
     frustum[0].z = clip[11] - clip[8];
     frustum[0].w = clip[15] - clip[12];

     frustum[1].x = clip[3] + clip[0];
     frustum[1].y = clip[7] + clip[4];
     frustum[1].z = clip[11] + clip[8];
     frustum[1].w = clip[15] + clip[12];

     frustum[2].x = clip[3] + clip[1];
     frustum[2].y = clip[7] + clip[5];
     frustum[2].z = clip[11] + clip[9];
     frustum[2].w = clip[15] + clip[13];

     frustum[3].x = clip[3] - clip[1];
     frustum[3].y = clip[7] - clip[5];
     frustum[3].z = clip[11] - clip[9];
     frustum[3].w = clip[15] - clip[13];

     frustum[4].x = clip[3] - clip[2];
     frustum[4].y = clip[7] - clip[6];
     frustum[4].z = clip[11] - clip[10];
     frustum[4].w = clip[15] - clip[14];

     frustum[5].x = clip[3] + clip[2];
     frustum[5].y = clip[7] + clip[6];
     frustum[5].z = clip[11] + clip[10];
     frustum[5].w = clip[15] + clip[14];

     for (int i = 0; i < 6; i++)
     {
         glm::normalize(frustum[i]);
     }
  }
  
  void DrawAxes(const float* view, const float* projection, const float* matrices, int matrixCount)
  {
      glm::mat4 viewM = *(const glm::mat4*)view;
      glm::mat4 projM = *(const glm::mat4*)projection;
      glm::mat4 viewProj = projM * viewM;

      glm::vec4 frustum[6];
      ComputeFrustumPlanes(frustum, glm::value_ptr(viewProj));

      glm::vec2 vpPos = glm::vec2(gContext.mX, gContext.mY);
      glm::vec2 vpSize = glm::vec2(gContext.mWidth, gContext.mHeight);

      for (int i = 0; i < matrixCount; i++)
      {
          const float* matrix = &matrices[i * 16];
          glm::mat4 model = *(const glm::mat4*)matrix;
          glm::mat4 mvp = viewProj * model;

          glm::vec4 origin = model * glm::vec4(0.f, 0.f, 0.f, 1.f);

          struct Axis
          {
              glm::vec4 dir;
              U32 color;
          };

          Axis axes[3] = {
                { glm::vec4(1.f, 0.f, 0.f, 0.f), 0xFF0000FF },
                { glm::vec4(0.f, 1.f, 0.f, 0.f), 0xFF00FF00 },
                { glm::vec4(0.f, 0.f, 1.f, 0.f), 0xFFFF0000 }
          };

          for (int a = 0; a < 3; a++)
          {
              glm::vec4 endLocal = axes[a].dir;
              glm::vec4 end = model * glm::vec4(glm::vec3(endLocal), 1.f);

              bool visible = true;
              for (int f = 0; f < 6; f++)
              {
                  float d0 = DistanceToPlane(origin, frustum[f]);
                  float d1 = DistanceToPlane(end, frustum[f]);

                  if (d0 < 0.f && d1 < 0.f)
                  {
                      visible = false;
                      break;
                  }
              }

              if (!visible)
              {
                  continue;
              }

              glm::vec2 p0 = worldToPos(glm::vec4(0.f, 0.f, 0.f, 1.f), mvp, vpPos, vpSize);
              glm::vec2 p1 = worldToPos(glm::vec4(glm::vec3(endLocal), 1.f), mvp, vpPos, vpSize);

              glm::vec4 clip0 = mvp * glm::vec4(0.f, 0.f, 0.f, 1.f);
              glm::vec4 clip1 = mvp * glm::vec4(glm::vec3(endLocal), 1.f);

              if (clip0.w <= 0.f && clip1.w <= 0.f)
              {
                  continue;
              }

              gContext.mDrawList->AddLine(p0, p1, axes[a].color, 2.0f);
          }
      }
  }

  void DrawCubes(const float* view, const float* projection, const float* matrices, int matrixCount)
  {
     glm::mat4 viewInverse;
     viewInverse = glm::inverse(*(glm::mat4*)view);

     struct CubeFace
     {
        float z;
        glm::vec2 faceCoordsScreen[4];
        U32 color;
     };
     CubeFace* faces = (CubeFace*)_malloca(sizeof(CubeFace) * matrixCount * 6);

     if (!faces)
     {
        return;
     }

     glm::vec4 frustum[6];
     glm::mat4 viewProjection = *(glm::mat4*)view * *(glm::mat4*)projection;
     ComputeFrustumPlanes(frustum, glm::value_ptr(viewProjection));

     int cubeFaceCount = 0;
     for (int cube = 0; cube < matrixCount; cube++)
     {
        const float* matrix = &matrices[cube * 16];

        glm::mat4 res = *(glm::mat4*)matrix * *(glm::mat4*)view * *(glm::mat4*)projection;

        for (int iFace = 0; iFace < 6; iFace++)
        {
           const int normalIndex = (iFace % 3);
           const int perpXIndex = (normalIndex + 1) % 3;
           const int perpYIndex = (normalIndex + 2) % 3;
           const float invert = (iFace > 2) ? -1.f : 1.f;

           const glm::vec4 faceCoords[4] = { directionUnary[normalIndex] + directionUnary[perpXIndex] + directionUnary[perpYIndex],
              directionUnary[normalIndex] + directionUnary[perpXIndex] - directionUnary[perpYIndex],
              directionUnary[normalIndex] - directionUnary[perpXIndex] - directionUnary[perpYIndex],
              directionUnary[normalIndex] - directionUnary[perpXIndex] + directionUnary[perpYIndex],
           };

           // clipping
           /*
           bool skipFace = false;
           for (unsigned int iCoord = 0; iCoord < 4; iCoord++)
           {
              glm::vec4 camSpacePosition;
              camSpacePosition.TransformPoint(faceCoords[iCoord] * 0.5f * invert, res);
              if (camSpacePosition.z < 0.001f)
              {
                 skipFace = true;
                 break;
              }
           }
           if (skipFace)
           {
              continue;
           }
           */
           glm::vec4 centerPosition, centerPositionVP;
           centerPosition = *(const glm::mat4*)matrix * glm::vec4(glm::vec3(directionUnary[normalIndex]) * 0.5f * invert, 1.0f);
           centerPositionVP = res * glm::vec4(glm::vec3(directionUnary[normalIndex]) * 0.5f * invert, 1.0f);

           bool inFrustum = true;
           for (int iFrustum = 0; iFrustum < 6; iFrustum++)
           {
               const glm::vec4& plane = frustum[iFrustum];

               bool allOutside = true;

               for (unsigned int iCoord = 0; iCoord < 4; iCoord++)
               {
                   glm::vec4 worldPos = *(const glm::mat4*)matrix * glm::vec4(glm::vec3(faceCoords[iCoord]) * 0.5f * invert, 1.0f);

                   if (DistanceToPlane(worldPos, plane) >= 0.f)
                   {
                       allOutside = false;
                       break;
                   }
               }

              if (allOutside)
              {
                 continue; // face is fully outside this plane - discard
              }
           }

           if (!inFrustum)
           {
              continue;
           }
           CubeFace& cubeFace = faces[cubeFaceCount];

           // 3D->2D
           //glm::vec2 faceCoordsScreen[4];
           for (unsigned int iCoord = 0; iCoord < 4; iCoord++)
           {
              cubeFace.faceCoordsScreen[iCoord] = worldToPos(faceCoords[iCoord] * 0.5f * invert, res);
           }

           U32 directionColor = GetColorU32(DIRECTION_X + normalIndex);
           cubeFace.color = directionColor | _COL32(0x80, 0x80, 0x80, 0);

           cubeFace.z = centerPositionVP.z / centerPositionVP.w;
           cubeFaceCount++;
        }
     }
     qsort(faces, cubeFaceCount, sizeof(CubeFace), [](void const* _a, void const* _b) {
        CubeFace* a = (CubeFace*)_a;
        CubeFace* b = (CubeFace*)_b;
        if (a->z < b->z)
        {
           return 1;
        }
        return -1;
        });
     // draw face with lighter color
     for (int iFace = 0; iFace < cubeFaceCount; iFace++)
     {
        const CubeFace& cubeFace = faces[iFace];
        gContext.mDrawList->AddConvexPolyFilled(cubeFace.faceCoordsScreen, 4, cubeFace.color);
     }

     _freea(faces);
  }

  void DrawGrid(const float* view, const float* projection, const float* matrix, const float gridSize)
  {
     glm::mat4 viewProjection = *(glm::mat4*)projection * *(glm::mat4*)view;
     glm::vec4 frustum[6];
     ComputeFrustumPlanes(frustum, glm::value_ptr(viewProjection));
     glm::mat4 res = viewProjection * *(glm::mat4*)matrix;
     glm::vec4 origin = res * glm::vec4(0, 0, 0, 1);

     //std::cout
     //    << origin.x << " "
     //    << origin.y << " "
     //    << origin.z << " "
     //    << origin.w
     //    << std::endl;
     for (float f = -gridSize; f <= gridSize; f += 1.f)
     {
        for (int dir = 0; dir < 2; dir++)
        {
           glm::vec4 ptA = makeVect(dir ? -gridSize : f, 0.f, dir ? f : -gridSize);
           glm::vec4 ptB = makeVect(dir ? gridSize : f, 0.f, dir ? f : gridSize);
           bool visible = true;
           for (int i = 0; i < 6; i++)
           {
              float dA = DistanceToPlane(ptA, frustum[i]);
              float dB = DistanceToPlane(ptB, frustum[i]);
              if (dA < 0.f && dB < 0.f)
              {
                 visible = false;
                 break;
              }
              if (dA > 0.f && dB > 0.f)
              {
                 continue;
              }
              if (dA < 0.f)
              {
                  float len = fabsf(dA - dB);
                  float t = (len > FLT_EPSILON) ? (fabsf(dA) / len) : 0.f;
                  ptA = glm::mix(ptA, ptB, t);
              }
              if (dB < 0.f)
              {
                  float len = fabsf(dB - dA);
                  float t = (len > FLT_EPSILON) ? (fabsf(dB) / len) : 0.f;
                  ptB = glm::mix(ptB, ptA, t);
              }
           }
           if (visible)
           {
              U32 col = _COL32(0x80, 0x80, 0x80, 0xFF);
              col = (fmodf(fabsf(f), 10.f) < FLT_EPSILON) ? _COL32(0x90, 0x90, 0x90, 0xFF) : col;
              col = (fabsf(f) < FLT_EPSILON) ? _COL32(0x40, 0x40, 0x40, 0xFF): col;

              float thickness = 1.f;
              thickness = (fmodf(fabsf(f), 10.f) < FLT_EPSILON) ? 1.5f : thickness;
              thickness = (fabsf(f) < FLT_EPSILON) ? 2.3f : thickness;

              gContext.mDrawList->AddLine(worldToPos(ptA, res), worldToPos(ptB, res), col, thickness);
           }
        }
     }
  }

  void DrawGridCustom(const float* view, const float* projection, const float* matrix, const float gridSize, const float majorStep, const unsigned int subdivision)
  {
     const U32 majorCol = _COL32(0x80, 0x80, 0x80, 0xFF);
     const U32 minorCol = _COL32(0x90, 0x90, 0x90, 0xFF);
     const U32 centerCol = _COL32(0x40, 0x40, 0x40, 0xFF);
     DrawGridCustomColor(view, projection, matrix, gridSize, majorStep, subdivision, majorCol, minorCol, centerCol);
  }

  void DrawGridCustomColor(const float* view, const float* projection, const float* matrix, const float gridSize, const float majorStep, const unsigned int subdivision, const U32 majorCol, const U32 minorCol, const U32 centerCol)
  {
      // Must have at least 1 subdivision
      _ASSERT(subdivision > 0 && "At least 1 segment required!");

      glm::mat4 viewProjection = *(glm::mat4*)view * *(glm::mat4*)projection;

      glm::vec4 frustum[6];
      ComputeFrustumPlanes(frustum, glm::value_ptr(viewProjection));

      glm::mat4 res = *(glm::mat4*)matrix * viewProjection;

      const float minorStep = majorStep / (float)subdivision;
      const int lineCount = (int)ceilf(gridSize / minorStep);

      for (int i = -lineCount; i <= lineCount; i++)
      {
          float f = i * minorStep;

          const bool isMajor  = (i % (int)subdivision) == 0;
          const bool isCenter = (i == 0);

          // Styling
          U32 col = minorCol;
          if (isMajor)
              col = majorCol;
          if (isCenter)
              col = centerCol;

          float thickness = 1.0f;
          if (isMajor)
              thickness = 1.5f;
          if (isCenter)
              thickness = 2.3f;

          for (int dir = 0; dir < 2; dir++)
          {
              glm::vec4 ptA = makeVect(dir ? -gridSize : f, 0.f, dir ? f : -gridSize);
              glm::vec4 ptB = makeVect(dir ? gridSize : f, 0.f, dir ? f : gridSize);

              bool visible = true;

              // Frustum clipping
              for (int p = 0; p < 6; p++)
              {
                  float dA = DistanceToPlane(ptA, frustum[p]);
                  float dB = DistanceToPlane(ptB, frustum[p]);

                  if (dA < 0.f && dB < 0.f)
                  {
                      visible = false;
                      break;
                  }

                  if (dA > 0.f && dB > 0.f)
                      continue;

                  if (dA < 0.f)
                  {
                      float len = fabsf(dA - dB);
                      float t = (len > FLT_EPSILON) ? (fabsf(dA) / len) : 0.f;
                      ptA = glm::mix(ptA, ptB, t);
                  }
                  if (dB < 0.f)
                  {
                      float len = fabsf(dB - dA);
                      float t = (len > FLT_EPSILON) ? (fabsf(dB) / len) : 0.f;
                      ptB = glm::mix(ptB, ptA, t);
                  }
              }

              if (visible)
              {
                  gContext.mDrawList->AddLine(
                      worldToPos(ptA, res),
                      worldToPos(ptB, res),
                      col,
                      thickness);
              }
          }
      }
  }

  void ViewManipulate(float* view, const float* projection, OPERATION operation, MODE mode, float* matrix, float length, glm::vec2 position, glm::vec2 size, U32 backgroundColor)
  {
     // Scale is always local or matrix will be skewed when applying world scale or oriented matrix
     ComputeContext(view, projection, matrix, (operation & SCALE) ? LOCAL : mode);
     ViewManipulate(view, length, position, size, backgroundColor);
  }

  void ViewManipulate(float* view, float length, glm::vec2 position, glm::vec2 size, U32 backgroundColor)
  {
      static bool isDraging = false;
      static bool isClicking = false;
      static glm::vec4 interpolationUp(0.f);
      static glm::vec4 interpolationDir(0.f);
      static int interpolationFrames = 0;
      const glm::vec4 referenceUp = glm::vec4(0.f, 1.f, 0.f, 0.f);

      glm::mat4 svgView = gContext.mViewMat;
      glm::mat4 svgProjection = gContext.mProjectionMat;

      IInput* input = gContext.mInput;
      if (!input) return;

      double mouseX, mouseY;
      input->getMousePosition(mouseX, mouseY);
      double mouseDx, mouseDy;
      input->getMouseDelta(mouseDx, mouseDy);

      gContext.mDrawList->AddRectFilled(position, position + size, backgroundColor);

      glm::mat4 viewMatrix = glm::make_mat4(view);
      glm::mat4 viewInverse = glm::inverse(viewMatrix);

      const bool rightHanded = gContext.mProjectionMat[2][3] < 0.f;
      const float handednessSign = rightHanded ? 1.f : -1.f;

      glm::vec4 camDir = viewInverse[2];
      glm::vec4 camTarget = viewInverse[3] - camDir * handednessSign * length;

      const float distance = 3.f;
      glm::mat4 cubeProjection, cubeView;
      float fov = acosf(distance / (sqrtf(distance * distance + 3.f))) * RAD2DEG;

      Perspective(fov / sqrtf(2.f), size.x / size.y, 0.01f, 1000.f, glm::value_ptr(cubeProjection), rightHanded);

      glm::vec4 dir = glm::vec4(viewInverse[2][0], viewInverse[2][1], viewInverse[2][2], 0.f);
      glm::vec4 up = glm::vec4(viewInverse[1][0], viewInverse[1][1], viewInverse[1][2], 0.f);
      glm::vec4 eye = dir * handednessSign * distance;
      glm::vec4 zero = glm::vec4(0.f, 0.f, 0.f, 1.f);

      LookAt(&eye.x, &zero.x, &up.x, glm::value_ptr(cubeView), rightHanded);

      gContext.mViewMat = cubeView;
      gContext.mProjectionMat = cubeProjection;
      ComputeCameraRay(gContext.mRayOrigin, gContext.mRayVector, position, size);

      const glm::mat4 res = cubeProjection * cubeView;

      static const glm::vec2 panelPosition[9] = {
          glm::vec2(0.75f, 0.75f), glm::vec2(0.25f, 0.75f), glm::vec2(0.f, 0.75f),
          glm::vec2(0.75f, 0.25f), glm::vec2(0.25f, 0.25f), glm::vec2(0.f, 0.25f),
          glm::vec2(0.75f, 0.f),    glm::vec2(0.25f, 0.f),    glm::vec2(0.f, 0.f)
      };

      static const glm::vec2 panelSize[9] = {
          glm::vec2(0.25f, 0.25f), glm::vec2(0.5f, 0.25f), glm::vec2(0.25f, 0.25f),
          glm::vec2(0.25f, 0.5f),  glm::vec2(0.5f, 0.5f),  glm::vec2(0.25f, 0.5f),
          glm::vec2(0.25f, 0.25f), glm::vec2(0.5f, 0.25f), glm::vec2(0.25f, 0.25f)
      };

      bool boxes[27]{};
      static int overBox = -1;

      for (int iPass = 0; iPass < 2; iPass++)
      {
          for (int iFace = 0; iFace < 6; iFace++)
          {
              const int normalIndex = (iFace % 3);
              const int perpXIndex = (normalIndex + 1) % 3;
              const int perpYIndex = (normalIndex + 2) % 3;
              const float invert = (iFace > 2) ? -1.f : 1.f;

              const glm::vec4 indexVectorX = directionUnary[perpXIndex] * invert;
              const glm::vec4 indexVectorY = directionUnary[perpYIndex] * invert;
              const glm::vec4 boxOrigin = directionUnary[normalIndex] * -invert - indexVectorX - indexVectorY;

              const glm::vec4 n = directionUnary[normalIndex] * invert;

              glm::vec4 viewSpaceNormal = cubeView * n;
              viewSpaceNormal.w = 0.f;
              viewSpaceNormal = glm::normalize(viewSpaceNormal);

              if (viewSpaceNormal.z >= 0.0f) continue;

              glm::vec4 facePlan = glm::vec4(glm::vec3(n), -glm::dot(glm::vec3(n), glm::vec3(n * 0.5f)));

              const float len = IntersectRayPlane(gContext.mRayOrigin, gContext.mRayVector, facePlan);
              glm::vec4 posOnPlan = gContext.mRayOrigin + gContext.mRayVector * len - (n * 0.5f);

              float localx = glm::dot(posOnPlan, directionUnary[perpXIndex]) * invert + 0.5f;
              float localy = glm::dot(posOnPlan, directionUnary[perpYIndex]) * invert + 0.5f;

              const glm::vec4 dx = directionUnary[perpXIndex];
              const glm::vec4 dy = directionUnary[perpYIndex];
              const glm::vec4 origin = directionUnary[normalIndex] - dx - dy;

              for (int iPanel = 0; iPanel < 9; iPanel++)
              {
                  glm::vec4 boxCoord = boxOrigin + indexVectorX * float(iPanel % 3) + indexVectorY * float(iPanel / 3) + glm::vec4(1.f, 1.f, 1.f, 0.f);
                  const glm::vec2 p = panelPosition[iPanel] * 2.f;
                  const glm::vec2 s = panelSize[iPanel] * 2.f;

                  glm::vec2 faceCoordsScreen[4];
                  glm::vec4 panelPos[4] = {
                      dx * p.x + dy * p.y,
                      dx * p.x + dy * (p.y + s.y),
                      dx * (p.x + s.x) + dy * (p.y + s.y),
                      dx * (p.x + s.x) + dy * p.y
                  };

                  for (unsigned int iCoord = 0; iCoord < 4; iCoord++)
                  {
                      faceCoordsScreen[iCoord] = worldToPos((panelPos[iCoord] + origin) * 0.5f * invert, res, position, size);
                  }

                  const glm::vec2 panelCorners[2] = { panelPosition[iPanel], panelPosition[iPanel] + panelSize[iPanel] };
                  bool insidePanel = localx > panelCorners[0].x && localx < panelCorners[1].x && localy > panelCorners[0].y && localy < panelCorners[1].y;
                  int boxCoordInt = int(boxCoord.x * 9.f + boxCoord.y * 3.f + boxCoord.z);

                  boxes[boxCoordInt] |= insidePanel && (!isDraging) && gContext.mbMouseOver;

                  if (iPass)
                  {
                      U32 directionColor = GetColorU32(DIRECTION_X + normalIndex);
                      gContext.mDrawList->AddConvexPolyFilled(faceCoordsScreen, 4, (directionColor | _COL32(128, 128, 128, 128)));

                      if (boxes[boxCoordInt])
                      {
                          U32 selectionColor = GetColorU32(SELECTION);
                          gContext.mDrawList->AddConvexPolyFilled(faceCoordsScreen, 4, selectionColor);

                          if (input->getMouseButtonDown(MouseButton::Left) && !isClicking && !isDraging) {
                              overBox = boxCoordInt;
                              isClicking = true;
                              isDraging = true;
                          }
                      }
                  }
              }
          }
      }

      if (interpolationFrames)
      {
          interpolationFrames--;
          glm::vec4 newDir = viewInverse[2] * handednessSign;
          newDir = glm::mix(newDir, interpolationDir * handednessSign, 0.2f);
          newDir = glm::normalize(newDir);

          glm::vec4 newUp = viewInverse[1];
          newUp = glm::mix(newUp, interpolationUp, 0.3f);
          newUp = glm::normalize(newUp);

          glm::vec4 newEye = camTarget + newDir * length;
          LookAt(&newEye.x, &camTarget.x, &newUp.x, view, rightHanded);
      }

      gContext.mIsViewManipulatorHovered = gContext.mbMouseOver && Rect(position, position + size).Contains(glm::vec2(mouseX, mouseY));

      if (input->getMouseButton(MouseButton::Left) && (fabsf(static_cast<float>(mouseDx)) || fabsf(static_cast<float>(mouseDy))) && isClicking)
      {
          isClicking = false;
      }

      if (!input->getMouseButton(MouseButton::Left))
      {
          if (isClicking)
          {
              int cx = overBox / 9;
              int cy = (overBox - cx * 9) / 3;
              int cz = overBox % 3;
              interpolationDir = glm::vec4(1.f - (float)cx, 1.f - (float)cy, 1.f - (float)cz, 0.f) * handednessSign;
              interpolationDir = glm::normalize(interpolationDir);

              if (fabsf(glm::dot(interpolationDir, referenceUp)) > 1.0f - 0.01f)
              {
                  glm::vec4 right = viewInverse[0];
                  if (fabsf(right.x) > fabsf(right.z)) right.z = 0.f; else right.x = 0.f;
                  right = glm::normalize(right);
                  interpolationUp = referenceUp;
              }
              else
              {
                  glm::vec3 right = glm::cross(glm::vec3(referenceUp), glm::vec3(interpolationDir));
                  right = glm::normalize(right);
                  glm::vec3 upResult = glm::cross(glm::vec3(interpolationDir), right);
                  interpolationUp = glm::vec4(glm::normalize(upResult), 0.f);
              }
              interpolationFrames = 20;
              isClicking = false;
          }
          isDraging = false;
      }

      gContext.mViewMat = svgView;
      gContext.mProjectionMat = svgProjection;
  }

};
