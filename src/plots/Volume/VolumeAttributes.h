// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#ifndef VOLUMEATTRIBUTES_H
#define VOLUMEATTRIBUTES_H
#include <string>
#include <AttributeSubject.h>

#include <ColorControlPointList.h>
#include <GaussianControlPointList.h>

// ****************************************************************************
// Class: VolumeAttributes
//
// Purpose:
//    This class contains the plot attributes for the volume plot.
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   omitted
//
// Modifications:
//
// ****************************************************************************

class VolumeAttributes : public AttributeSubject
{
public:
    enum Renderer
    {
        Default,
        RayCasting,
        RayCastingIntegration,
        RayCastingSLIVR,
        RayCastingOSPRay
    };
    enum GradientType
    {
        CenteredDifferences,
        SobelOperator
    };
    enum Scaling
    {
        Linear,
        Log,
        Skew
    };
    enum LimitsMode
    {
        OriginalData,
        CurrentPlot
    };
    enum SamplingType
    {
        KernelBased,
        Rasterization,
        Trilinear
    };
    enum OpacityModes
    {
        FreeformMode,
        GaussianMode,
        ColorTableMode
    };
    enum LowGradientLightingReduction
    {
        Off,
        Lowest,
        Lower,
        Low,
        Medium,
        High,
        Higher,
        Highest
    };
    enum RenderMode
    {
        DefaultRenderMode,
        RayCastRenderMode,
        GPURenderMode,
        OSPRayRenderMode,
        AnariRenderMode
    };

    // These constructors are for objects of this class
    VolumeAttributes();
    VolumeAttributes(const VolumeAttributes &obj);
protected:
    // These constructors are for objects derived from this class
    VolumeAttributes(private_tmfs_t tmfs);
    VolumeAttributes(const VolumeAttributes &obj, private_tmfs_t tmfs);
public:
    virtual ~VolumeAttributes();

    virtual VolumeAttributes& operator = (const VolumeAttributes &obj);
    virtual bool operator == (const VolumeAttributes &obj) const;
    virtual bool operator != (const VolumeAttributes &obj) const;
private:
    void Init();
    void Copy(const VolumeAttributes &obj);
public:

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectColorControlPoints();
    void SelectOpacityControlPoints();
    void SelectOpacityVariable();
    void SelectCompactVariable();
    void SelectFreeformOpacity();
    void SelectMaterialProperties();

    // Property setting methods
    void SetOsprayShadowsEnabledFlag(bool osprayShadowsEnabledFlag_);
    void SetOsprayUseGridAcceleratorFlag(bool osprayUseGridAcceleratorFlag_);
    void SetOsprayPreIntegrationFlag(bool osprayPreIntegrationFlag_);
    void SetOspraySingleShadeFlag(bool ospraySingleShadeFlag_);
    void SetOsprayOneSidedLightingFlag(bool osprayOneSidedLightingFlag_);
    void SetOsprayAoTransparencyEnabledFlag(bool osprayAoTransparencyEnabledFlag_);
    void SetOspraySpp(int ospraySpp_);
    void SetOsprayAoSamples(int osprayAoSamples_);
    void SetOsprayAoDistance(double osprayAoDistance_);
    void SetOsprayMinContribution(double osprayMinContribution_);
    void SetLegendFlag(bool legendFlag_);
    void SetLightingFlag(bool lightingFlag_);
    void SetColorControlPoints(const ColorControlPointList &colorControlPoints_);
    void SetOpacityAttenuation(float opacityAttenuation_);
    void SetOpacityMode(OpacityModes opacityMode_);
    void SetOpacityControlPoints(const GaussianControlPointList &opacityControlPoints_);
    void SetResampleFlag(bool resampleFlag_);
    void SetResampleTarget(int resampleTarget_);
    void SetOpacityVariable(const std::string &opacityVariable_);
    void SetCompactVariable(const std::string &compactVariable_);
    void SetFreeformOpacity(const unsigned char *freeformOpacity_);
    void SetUseColorVarMin(bool useColorVarMin_);
    void SetColorVarMin(float colorVarMin_);
    void SetUseColorVarMax(bool useColorVarMax_);
    void SetColorVarMax(float colorVarMax_);
    void SetUseOpacityVarMin(bool useOpacityVarMin_);
    void SetOpacityVarMin(float opacityVarMin_);
    void SetUseOpacityVarMax(bool useOpacityVarMax_);
    void SetOpacityVarMax(float opacityVarMax_);
    void SetSmoothData(bool smoothData_);
    void SetSamplesPerRay(int samplesPerRay_);
    void SetRendererType(Renderer rendererType_);
    void SetGradientType(GradientType gradientType_);
    void SetScaling(Scaling scaling_);
    void SetSkewFactor(double skewFactor_);
    void SetLimitsMode(LimitsMode limitsMode_);
    void SetSampling(SamplingType sampling_);
    void SetRendererSamples(float rendererSamples_);
    void SetLowGradientLightingReduction(LowGradientLightingReduction lowGradientLightingReduction_);
    void SetLowGradientLightingClampFlag(bool lowGradientLightingClampFlag_);
    void SetLowGradientLightingClampValue(double lowGradientLightingClampValue_);
    void SetMaterialProperties(const double *materialProperties_);
    void SetRenderMode(RenderMode renderMode_);

    // Property getting methods
    bool                           GetOsprayShadowsEnabledFlag() const;
    bool                           GetOsprayUseGridAcceleratorFlag() const;
    bool                           GetOsprayPreIntegrationFlag() const;
    bool                           GetOspraySingleShadeFlag() const;
    bool                           GetOsprayOneSidedLightingFlag() const;
    bool                           GetOsprayAoTransparencyEnabledFlag() const;
    int                            GetOspraySpp() const;
    int                            GetOsprayAoSamples() const;
    double                         GetOsprayAoDistance() const;
    double                         GetOsprayMinContribution() const;
    bool                           GetLegendFlag() const;
    bool                           GetLightingFlag() const;
    const ColorControlPointList    &GetColorControlPoints() const;
          ColorControlPointList    &GetColorControlPoints();
    float                          GetOpacityAttenuation() const;
    OpacityModes                   GetOpacityMode() const;
    const GaussianControlPointList &GetOpacityControlPoints() const;
          GaussianControlPointList &GetOpacityControlPoints();
    bool                           GetResampleFlag() const;
    int                            GetResampleTarget() const;
    const std::string              &GetOpacityVariable() const;
          std::string              &GetOpacityVariable();
    const std::string              &GetCompactVariable() const;
          std::string              &GetCompactVariable();
    const unsigned char            *GetFreeformOpacity() const;
          unsigned char            *GetFreeformOpacity();
    bool                           GetUseColorVarMin() const;
    float                          GetColorVarMin() const;
    bool                           GetUseColorVarMax() const;
    float                          GetColorVarMax() const;
    bool                           GetUseOpacityVarMin() const;
    float                          GetOpacityVarMin() const;
    bool                           GetUseOpacityVarMax() const;
    float                          GetOpacityVarMax() const;
    bool                           GetSmoothData() const;
    int                            GetSamplesPerRay() const;
    Renderer                       GetRendererType() const;
    GradientType                   GetGradientType() const;
    Scaling                        GetScaling() const;
    double                         GetSkewFactor() const;
    LimitsMode                     GetLimitsMode() const;
    SamplingType                   GetSampling() const;
    float                          GetRendererSamples() const;
    LowGradientLightingReduction   GetLowGradientLightingReduction() const;
    bool                           GetLowGradientLightingClampFlag() const;
    double                         GetLowGradientLightingClampValue() const;
    const double                   *GetMaterialProperties() const;
          double                   *GetMaterialProperties();
    RenderMode                     GetRenderMode() const;

    // Persistence methods
    virtual bool CreateNode(DataNode *node, bool completeSave, bool forceAdd);
    virtual void SetFromNode(DataNode *node);

    // Enum conversion functions
    static std::string Renderer_ToString(Renderer);
    static bool Renderer_FromString(const std::string &, Renderer &);
protected:
    static std::string Renderer_ToString(int);
public:
    static std::string GradientType_ToString(GradientType);
    static bool GradientType_FromString(const std::string &, GradientType &);
protected:
    static std::string GradientType_ToString(int);
public:
    static std::string Scaling_ToString(Scaling);
    static bool Scaling_FromString(const std::string &, Scaling &);
protected:
    static std::string Scaling_ToString(int);
public:
    static std::string LimitsMode_ToString(LimitsMode);
    static bool LimitsMode_FromString(const std::string &, LimitsMode &);
protected:
    static std::string LimitsMode_ToString(int);
public:
    static std::string SamplingType_ToString(SamplingType);
    static bool SamplingType_FromString(const std::string &, SamplingType &);
protected:
    static std::string SamplingType_ToString(int);
public:
    static std::string OpacityModes_ToString(OpacityModes);
    static bool OpacityModes_FromString(const std::string &, OpacityModes &);
protected:
    static std::string OpacityModes_ToString(int);
public:
    static std::string LowGradientLightingReduction_ToString(LowGradientLightingReduction);
    static bool LowGradientLightingReduction_FromString(const std::string &, LowGradientLightingReduction &);
protected:
    static std::string LowGradientLightingReduction_ToString(int);
public:
    static std::string RenderMode_ToString(RenderMode);
    static bool RenderMode_FromString(const std::string &, RenderMode &);
protected:
    static std::string RenderMode_ToString(int);
public:

    // Keyframing methods
    virtual std::string               GetFieldName(int index) const;
    virtual AttributeGroup::FieldType GetFieldType(int index) const;
    virtual std::string               GetFieldTypeName(int index) const;
    virtual bool                      FieldsEqual(int index, const AttributeGroup *rhs) const;

    // User-defined methods
    bool ChangesRequireRecalculation(const VolumeAttributes &obj) const;
    void GetTransferFunction(unsigned char *rgba) const;
    void SetDefaultColorControlPoints();
    void GetGaussianOpacities(unsigned char *alphas) const;
    void GetOpacities(unsigned char *alphas);
    void SetSmoothing(ColorControlPointList::SmoothingMethod val);
    ColorControlPointList::SmoothingMethod GetSmoothing() const;
    void SetEqualSpacingFlag(bool val);
    bool GetEqualSpacingFlag() const;
    bool AnyNonTransferFunctionMembersAreDifferent(const VolumeAttributes &) const;

    // IDs that can be used to identify fields in case statements
    enum {
        ID_osprayShadowsEnabledFlag = 0,
        ID_osprayUseGridAcceleratorFlag,
        ID_osprayPreIntegrationFlag,
        ID_ospraySingleShadeFlag,
        ID_osprayOneSidedLightingFlag,
        ID_osprayAoTransparencyEnabledFlag,
        ID_ospraySpp,
        ID_osprayAoSamples,
        ID_osprayAoDistance,
        ID_osprayMinContribution,
        ID_legendFlag,
        ID_lightingFlag,
        ID_colorControlPoints,
        ID_opacityAttenuation,
        ID_opacityMode,
        ID_opacityControlPoints,
        ID_resampleFlag,
        ID_resampleTarget,
        ID_opacityVariable,
        ID_compactVariable,
        ID_freeformOpacity,
        ID_useColorVarMin,
        ID_colorVarMin,
        ID_useColorVarMax,
        ID_colorVarMax,
        ID_useOpacityVarMin,
        ID_opacityVarMin,
        ID_useOpacityVarMax,
        ID_opacityVarMax,
        ID_smoothData,
        ID_samplesPerRay,
        ID_rendererType,
        ID_gradientType,
        ID_scaling,
        ID_skewFactor,
        ID_limitsMode,
        ID_sampling,
        ID_rendererSamples,
        ID_lowGradientLightingReduction,
        ID_lowGradientLightingClampFlag,
        ID_lowGradientLightingClampValue,
        ID_materialProperties,
        ID_renderMode,
        ID__LAST
    };

private:
    bool                     osprayShadowsEnabledFlag;
    bool                     osprayUseGridAcceleratorFlag;
    bool                     osprayPreIntegrationFlag;
    bool                     ospraySingleShadeFlag;
    bool                     osprayOneSidedLightingFlag;
    bool                     osprayAoTransparencyEnabledFlag;
    int                      ospraySpp;
    int                      osprayAoSamples;
    double                   osprayAoDistance;
    double                   osprayMinContribution;
    bool                     legendFlag;
    bool                     lightingFlag;
    ColorControlPointList    colorControlPoints;
    float                    opacityAttenuation;
    int                      opacityMode;
    GaussianControlPointList opacityControlPoints;
    bool                     resampleFlag;
    int                      resampleTarget;
    std::string              opacityVariable;
    std::string              compactVariable;
    unsigned char            freeformOpacity[256];
    bool                     useColorVarMin;
    float                    colorVarMin;
    bool                     useColorVarMax;
    float                    colorVarMax;
    bool                     useOpacityVarMin;
    float                    opacityVarMin;
    bool                     useOpacityVarMax;
    float                    opacityVarMax;
    bool                     smoothData;
    int                      samplesPerRay;
    int                      rendererType;
    int                      gradientType;
    int                      scaling;
    double                   skewFactor;
    int                      limitsMode;
    int                      sampling;
    float                    rendererSamples;
    int                      lowGradientLightingReduction;
    bool                     lowGradientLightingClampFlag;
    double                   lowGradientLightingClampValue;
    double                   materialProperties[4];
    int                      renderMode;

    // Static class format string for type map.
    static const char *TypeMapFormatString;
    static const private_tmfs_t TmfsStruct;
};
#define VOLUMEATTRIBUTES_TMFS "bbbbbbiiddbbafiabissUbfbfbfbfbiiiidiifibdDi"

#endif
