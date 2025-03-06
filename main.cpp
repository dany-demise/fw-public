#include <emscripten/bind.h>
#include "FilmicCurve/FilmicColorGrading.h"
#include "Core/Vec3.h"

emscripten::val vec3_js(float r, float g, float b) {
    emscripten::val js_array = emscripten::val::array();
    js_array.call<void>("push", r);  // Push each element into the js array
    js_array.call<void>("push", g);
    js_array.call<void>("push", b);
    return js_array;
}

// Wrapper class for user interaction
class FilmicWrapper {
public:
    FilmicWrapper() = default;

    void refreshParams() {
        // Convert user params to raw, eval, and baked params
        FilmicColorGrading::RawFromUserParams(rawParams, userParams);
        FilmicColorGrading::EvalFromRawParams(evalParams, rawParams);
        FilmicColorGrading::BakeFromEvalParams(bakeParams, evalParams, 256, FilmicColorGrading::kTableSpacing_Quadratic);
    }

    void resetUserParams() {
        userParams.Reset();
        refreshParams();
    }

    void setUserParams(
        float saturation,
        float exposureBias,
        float contrast,
        float toeStrength,
        float toeLength,
        float shoulderStrength,
        float shoulderLength,
        float shoulderAngle,
        float gamma,
        float postGamma
    ) {
        userParams.m_saturation = saturation;
        userParams.m_exposureBias = exposureBias;

        // no contrast midpoint, hardcoded to .18f
        // no contrast epislon, hardcoded to 1e-5f
        userParams.m_contrast = contrast;

        // filmic tonemapping
        userParams.m_filmicToeStrength = toeStrength;
        userParams.m_filmicToeLength = toeLength;
        userParams.m_filmicShoulderStrength = shoulderStrength;
        userParams.m_filmicShoulderLength = shoulderLength;
        userParams.m_filmicShoulderAngle = shoulderAngle;
        userParams.m_filmicGamma = gamma;

        userParams.m_postGamma = postGamma;

        refreshParams();
    }

    emscripten::val evalColor(float r, float g, float b) {
        Vec3 srcColor(r, g, b);
        Vec3 dstColor = bakeParams.EvalColor(srcColor);
        return vec3_js(dstColor.x, dstColor.y, dstColor.z);
    }

    emscripten::val getShaderParams() {
        emscripten::val shader_params = emscripten::val::object();
        shader_params.set( "linColorFilterExposure",
            vec3_js(
                bakeParams.m_linColorFilterExposure.x,
                bakeParams.m_linColorFilterExposure.y,
                bakeParams.m_linColorFilterExposure.z
            )
        );
        shader_params.set( "luminanceWeights",
            vec3_js(
                bakeParams.m_luminanceWeights.x,
                bakeParams.m_luminanceWeights.y,
                bakeParams.m_luminanceWeights.z
            )
        );
        shader_params.set( "saturation", bakeParams.m_saturation );
        return shader_params;
    }

    emscripten::val getRgbCurves() {
        // m_curveR
        emscripten::val rgb_array = emscripten::val::array();
        for (std::size_t i = 0; i < bakeParams.m_curveSize; ++i) {
            emscripten::val val = emscripten::val::array();
            val.call<void>("push", bakeParams.m_curveR[i]);
            val.call<void>("push", bakeParams.m_curveG[i]);
            val.call<void>("push", bakeParams.m_curveB[i]);
            rgb_array.call<void>("push", val);
        }
        return rgb_array;
    }

private:
    FilmicColorGrading::UserParams userParams;
    FilmicColorGrading::RawParams rawParams;
    FilmicColorGrading::EvalParams evalParams;
    FilmicColorGrading::BakedParams bakeParams;
};

// Binding code for Emscripten
EMSCRIPTEN_BINDINGS(FilmicWrapper) {
    emscripten::class_<FilmicWrapper>("FilmicWrapper")
        .constructor()
        .function("resetUserParams", &FilmicWrapper::resetUserParams)
        .function("setUserParams", &FilmicWrapper::setUserParams)
        .function("evalColor", &FilmicWrapper::evalColor)
        .function("getShaderParams", &FilmicWrapper::getShaderParams)
        .function("getRgbCurves", &FilmicWrapper::getRgbCurves);
}
