// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#ifndef ANARI_RENDERING_WIDGET_H
#define ANARI_RENDERING_WIDGET_H

#include <gui_exports.h>
#include <QWidget>

#include <anari/anari_cpp.hpp>

#include <vector>
#include <memory>

class RenderingAttributes;
class QvisRenderingWindow;
class QGroupBox;
class QComboBox;
class QSpinBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QStackedLayout;

namespace anari
{
    enum class BackendType
    {
        NONE,
        EXAMPLE,
        USD,
        VISRTX,
        VISGL,
        OSPRAY,
        RADEONPRORENDER
    };

    enum class USDParameter
    {
        COMMIT,
        BINARY,
        MATERIAL,
        PREVIEW,
        MDL,
        MDLCOLORS,
        DISPLAY
    };
}

using BackendType = anari::BackendType;
using USDParameter = anari::USDParameter;

class GUI_API AnariRenderingWidget : public QWidget
{
    Q_OBJECT
public:
    AnariRenderingWidget(QvisRenderingWindow *,
                         RenderingAttributes *,
                         QWidget *parent = nullptr);
    ~AnariRenderingWidget() = default;

    int GetRowCount() const { return totalRows; }

    // General
    void SetChecked(const bool);
    void UpdateLibraryName(const std::string);
    void UpdateLibrarySubtypes(const std::string);
    void UpdateRendererSubtypes(const std::string);

    // Back-end
    void UpdateSamplesPerPixel(const int);
    void UpdateAOSamples(const int);
    void UpdateLightFalloff(const float);
    void UpdateAmbientIntensity(const float);
    void UpdateMaxDepth(const int);
    void UpdateRValue(const float);
    void UpdateDebugMethod(const std::string);
    void UpdateDenoiserSelection(const bool);

    // USD Back-end
    void UpdateUSDOutputLocation(const std::string);
    void UpdateUSDParameter(const USDParameter, const bool);

signals:
    void currentBackendChanged(int);

private slots:
    void renderingToggled(bool);
    void libraryChanged();
    void librarySubtypeChanged(const QString &);
    void rendererSubtypeChanged(const QString &);

    // General
    void samplesPerPixelChanged(int);
    void aoSamplesChanged(int);
    void denoiserToggled(bool);
    void lightFalloffChanged();
    void ambientIntensityChanged();
    void maxDepthChanged(int);
    void rValueChanged();
    void debugMethodChanged(const QString &);

    // USD
    void outputLocationChanged();
    void selectButtonPressed();
    void commitToggled(bool);
    void binaryToggled(bool);
    void materialToggled(bool);
    void previewSurfaceToggled(bool);
    void mdlToggled(bool);
    void mdlColorsToggled(bool);
    void displayColorsToggled(bool);

private:
    QWidget *CreateGeneralWidget(int &);
    QWidget *CreateBackendWidget(int &);
    QWidget *CreateUSDWidget(int &);

    BackendType GetBackendType(const std::string &) const;

    void UpdateUI();
    void UpdateRendererParams(const std::string &, anari::Device anariDevice = nullptr);

    QvisRenderingWindow *renderingWindow;
    RenderingAttributes *renderingAttributes;
    QStackedLayout *backendStackedLayout;

    std::unique_ptr<std::vector<std::string>> rendererParams;
    int totalRows;

    // General Widget Components
    QGroupBox   *renderingGroup;
    QLineEdit   *libraryName;
    QComboBox   *librarySubtypes;
    QComboBox   *rendererSubtypes;

    // Backend widget UI components
    QSpinBox    *samplesPerPixel;
    QSpinBox    *aoSamples;
    QLineEdit   *lightFalloff;
    QLineEdit   *ambientIntensity;
    QSpinBox    *maxDepth;
    QLineEdit   *rValue;
    QComboBox   *debugMethod;
    QCheckBox   *denoiserToggle;

    // USD widget UI components
    std::unique_ptr<QString>    outputDir;

    QLineEdit   *dirLineEdit;
    QCheckBox   *commitCheckBox;
    QCheckBox   *binaryCheckBox;
    QCheckBox   *materialCheckBox;
    QCheckBox   *previewCheckBox;
    QCheckBox   *mdlCheckBox;
    QCheckBox   *mdlColorCheckBox;
    QCheckBox   *displayColorCheckBox;
};

#endif