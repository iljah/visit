// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <QvisSurfacePlotWindow.h>

#include <math.h>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>

#include <QvisColorButton.h>
#include <QvisColorTableWidget.h>
#include <QvisLineWidthWidget.h>
#include <SurfaceAttributes.h>
#include <ViewerProxy.h>

#include <DebugStream.h>

// ****************************************************************************
// Method: QvisSurfacePlotWindow::QvisSurfacePlotWindow
//
// Purpose: 
//   Constructor for the QvisSurfacePlotWindow class.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001 
//
// Modifications:
//    Eric Brugger, Fri Mar 16 16:17:18 PST 2001
//    I added the argument type.
//
// ****************************************************************************

QvisSurfacePlotWindow::QvisSurfacePlotWindow(const int type,
    SurfaceAttributes *surfaceAtts_, const QString &caption,
    const QString &shortName, QvisNotepadArea *notepad) :
    QvisPostableWindowObserver(surfaceAtts_, caption, shortName, notepad)
{
    plotType    = type;
    surfaceAtts = surfaceAtts_;
    colorModeButtons = 0;
    scalingButtons = 0;
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::~QvisSurfacePlotWindow
//
// Purpose: 
//   Destructor for the QvisSurfacePlotWindow class.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001 
//
// Modifications:
//   
// ****************************************************************************

QvisSurfacePlotWindow::~QvisSurfacePlotWindow()
{
    surfaceAtts = 0;
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::CreateWindowContents
//
// Purpose: 
//   This method creates the widgets that are in the window and sets
//   up their signals/slots.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001 
//
// Modifications:
//   Brad Whitlock, Sat Jun 16 18:28:35 PST 2001
//   Added a color table button.
//
//   Jeremy Meredith, Fri Sep 14 17:02:31 PDT 2001
//   Swapped the creation order of the wireframe label and button so the
//   button was created by the time the label wanted to buddy it.
//
//   Kathleen Bonnell, Thu Oct 11 12:45:30 PDT 2001 
//   Added limitsSelect button and associated colorinMin/Max.  Changed old
//   min/max to scalingMin/max. 
//
//   Brad Whitlock, Fri Oct 26 16:47:44 PST 2001
//   Enabled wireframe color.
//
//   Kathleen Bonnell, Thu Mar 28 14:03:19 PST 2002
//   Once again use the same limits for coloring and for scaling.  Removed
//   coloringMin/Max.  Renamed scalingMin/Max to simply min/max.
//   Moved limitsSelect to same window area as limits line edits. 
//
//   Kathleen Bonnell, Wed May 29 13:40:22 PDT 2002  
//   Remove 'Specify' from limitSelect. 
//
//   Brad Whitlock, Wed Apr 23 12:01:18 PDT 2008
//   Added tr()'s
//
//   Brad Whitlock, Fri Jul 18 11:17:41 PDT 2008
//   Qt 4.
//
//   Dave Pugmire, Wed Oct 29 16:00:48 EDT 2008
//   Swap the min/max in the gui.
//
//   Allen Sanderson, Sun Mar  7 12:49:56 PST 2010
//   Change layout of window for 2.0 interface changes.
//
//   Kathleen Bonnell, Mon Jan 17 17:54:48 MST 2011
//   Change colorTableButton to colorTableWidget to gain invert toggle.
//
//   Kathleen Biagas, Tue Apr 18 16:34:41 PDT 2023
//   Support Qt6: buttonClicked -> idClicked.
//
// ****************************************************************************

void
QvisSurfacePlotWindow::CreateWindowContents()
{
    //
    // Create the scale group
    //
    QGroupBox * dataGroup = new QGroupBox(central);
    dataGroup->setTitle(tr("Data"));
    topLayout->addWidget(dataGroup);

    QGridLayout *dataLayout = new QGridLayout(dataGroup);
    dataLayout->setContentsMargins(5,5,5,5);
    dataLayout->setSpacing(10);

    //
    // Create the scale radio buttons
    //
    dataLayout->addWidget( new QLabel(tr("Scale"), central), 0, 0);
    
    // Create the radio buttons
    scalingButtons = new QButtonGroup(central);

    QRadioButton * rb = new QRadioButton(tr("Linear"), central);
    rb->setChecked(true);
    scalingButtons->addButton(rb, 0);
    dataLayout->addWidget(rb, 0, 1);
    rb = new QRadioButton(tr("Log"), central);
    scalingButtons->addButton(rb, 1);
    dataLayout->addWidget(rb, 0, 2);
    rb = new QRadioButton(tr("Skew factor"), central);
    scalingButtons->addButton(rb, 2);
    dataLayout->addWidget(rb, 0, 3);

    // Each time a radio button is clicked, call the scale clicked slot.
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    connect(scalingButtons, SIGNAL(buttonClicked(int)),
            this, SLOT(scaleClicked(int)));
#else
    connect(scalingButtons, SIGNAL(idClicked(int)),
            this, SLOT(scaleClicked(int)));
#endif

    // Create the skew factor line edit    
    skewLineEdit = new QLineEdit(central);
    dataLayout->addWidget(skewLineEdit, 0, 4);
    connect(skewLineEdit, SIGNAL(returnPressed()),
            this, SLOT(processSkewText()));


    //
    // Create the Limits stuff
    //
    QGroupBox * limitsGroup = new QGroupBox(central);
    dataLayout->addWidget(limitsGroup, 2, 0, 1, 5);

    QGridLayout *limitsLayout = new QGridLayout(limitsGroup);
    limitsLayout->setContentsMargins(5,5,5,5);
    limitsLayout->setSpacing(10);

    limitsLayout->addWidget( new QLabel(tr("Limits"), central), 0, 0);

    limitsSelect = new QComboBox(central);
    limitsSelect->addItem(tr("Use Original Data"));
    limitsSelect->addItem(tr("Use Current Plot"));
    connect(limitsSelect, SIGNAL(activated(int)),
            this, SLOT(limitsSelectChanged(int))); 
    limitsLayout->addWidget(limitsSelect, 0, 1, 1, 2, Qt::AlignLeft);

    // Create the min toggle and line edit
    minToggle = new QCheckBox(tr("Minimum"), central);
    limitsLayout->addWidget(minToggle, 1, 0);
    connect(minToggle, SIGNAL(toggled(bool)),
            this, SLOT(minToggled(bool)));
    minLineEdit = new QLineEdit(central);
    connect(minLineEdit, SIGNAL(returnPressed()),
            this, SLOT(processMinLimitText())); 
    limitsLayout->addWidget(minLineEdit, 1, 1);

    // Create the max toggle and line edit
    maxToggle = new QCheckBox(tr("Maximum"), central);
    limitsLayout->addWidget(maxToggle, 1, 2);
    connect(maxToggle, SIGNAL(toggled(bool)),
            this, SLOT(maxToggled(bool)));
    maxLineEdit = new QLineEdit(central);
    connect(maxLineEdit, SIGNAL(returnPressed()),
            this, SLOT(processMaxLimitText())); 
    limitsLayout->addWidget(maxLineEdit, 1, 3);


    //
    // Create the surface color controls
    //
    surfaceGroup = new QGroupBox(tr("Surface"), central);
    surfaceGroup->setCheckable(true);
    connect(surfaceGroup, SIGNAL(toggled(bool)),
            this, SLOT(surfaceToggled(bool)));
    topLayout->addWidget(surfaceGroup);
 

    QGridLayout *surfaceLayout = new QGridLayout(surfaceGroup);
    surfaceLayout->setContentsMargins(5,5,5,5);
    surfaceLayout->setSpacing(10);
    surfaceLayout->setColumnStretch(1, 10);

    colorModeButtons = new QButtonGroup(surfaceGroup);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    connect(colorModeButtons, SIGNAL(buttonClicked(int)),
            this, SLOT(colorModeChanged(int)));
#else
    connect(colorModeButtons, SIGNAL(idClicked(int)),
            this, SLOT(colorModeChanged(int)));
#endif

    rb = new QRadioButton(tr("Color by Z value"), surfaceGroup);
    colorModeButtons->addButton(rb, 0);
    surfaceLayout->addWidget(rb, 0, 0);

    rb = new QRadioButton(tr("Constant color"), surfaceGroup);
    colorModeButtons->addButton(rb, 1);
    surfaceLayout->addWidget(rb, 1, 0);

    // Create the surface color-by-z button.
    colorTableWidget = new QvisColorTableWidget(surfaceGroup, true);
    connect(colorTableWidget, SIGNAL(selectedColorTable(bool, const QString &)),
            this, SLOT(colorTableClicked(bool, const QString &)));
    connect(colorTableWidget,
            SIGNAL(invertColorTableToggled(bool)),
            this,
            SLOT(invertColorTableToggled(bool)));
    surfaceLayout->addWidget(colorTableWidget, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);

    // Create the surface color button.
    surfaceColor = new QvisColorButton(surfaceGroup);
    surfaceColor->setButtonColor(QColor(255, 0, 0));
    connect(surfaceColor, SIGNAL(selectedColor(const QColor &)),
            this, SLOT(surfaceColorChanged(const QColor &)));
    surfaceLayout->addWidget(surfaceColor, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    //
    // Create the wireframe controls
    //
    wireframeGroup = new QGroupBox(tr("Wireframe"), central);
    wireframeGroup->setCheckable(true);
    connect(wireframeGroup, SIGNAL(toggled(bool)),
            this, SLOT(wireframeToggled(bool)));
    topLayout->addWidget(wireframeGroup);
 
    QGridLayout *wireframeLayout = new QGridLayout(wireframeGroup);
    wireframeLayout->setContentsMargins(5,5,5,5);
    wireframeLayout->setSpacing(10);

    // Create the lineWidth widget.
    lineWidth = new QvisLineWidthWidget(0, wireframeGroup);
    wireframeLayout->addWidget(lineWidth, 0, 1);
    connect(lineWidth, SIGNAL(lineWidthChanged(int)),
            this, SLOT(lineWidthChanged(int)));
    QLabel *lineWidthLabel = new QLabel(tr("Line width"), wireframeGroup);
    lineWidthLabel->setBuddy(lineWidth);
    wireframeLayout->addWidget(lineWidthLabel, 0, 0, Qt::AlignRight);

    // Create the wire frame color widget.
    wireframeColor = new QvisColorButton(wireframeGroup);
    wireframeColor->setButtonColor(QColor(0, 0, 0));
    connect(wireframeColor, SIGNAL(selectedColor(const QColor &)),
            this, SLOT(wireframeColorChanged(const QColor &)));
    wireframeLayout->addWidget(wireframeColor, 0, 4);

    QLabel *wireframeLabel = new QLabel(tr("Wire color"), wireframeGroup);
    wireframeLabel->setBuddy(wireframeColor);
    wireframeLayout->addWidget(wireframeLabel, 0, 3, Qt::AlignRight);

    //
    // Create the misc stuff
    //
    QGroupBox * miscGroup = new QGroupBox(central);
    miscGroup->setTitle(tr("Misc"));
    topLayout->addWidget(miscGroup);

    QGridLayout *miscLayout = new QGridLayout(miscGroup);
    miscLayout->setContentsMargins(5,5,5,5);
    miscLayout->setSpacing(10);
 
    // Create the legend toggle
    legendToggle = new QCheckBox(tr("Legend"), central);
    connect(legendToggle, SIGNAL(toggled(bool)),
            this, SLOT(legendToggled(bool)));
    miscLayout->addWidget(legendToggle, 0, 0);

    // Create the lighting toggle
    lightingToggle = new QCheckBox(tr("Lighting"), central);
    connect(lightingToggle, SIGNAL(toggled(bool)),
            this, SLOT(lightingToggled(bool)));
    miscLayout->addWidget(lightingToggle, 0, 1);
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::UpdateWindow
//
// Purpose: 
//   This method is called when the window's subject is changed. The
//   subject tells this window what attributes changed and we put the
//   new values into those widgets.
//
// Arguments:
//   doAll : If this flag is true, update all the widgets regardless
//           of whether or not they are selected.
//
// Returns:    
//
// Note:       
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001 
//
// Modifications:
//   Kathleen Bonnell, Mon Mar 26 18:17:53 PST 2001
//   Disable lineStyle and lineStyleLabel until we have vtk version
//   in which line stippling is available.
//
//   Brad Whitlock, Sat Jun 16 18:29:08 PST 2001
//   Added code to set the color table.
//
//   Kathleen Bonnell, Thu Jun 21 16:33:54 PDT 2001
//   Enable lineStyle and lineStyleLabel.
//
//   Kathleen Bonnell, Thu Oct 11 12:45:30 PDT 2001 
//   Added limitsSelect button and associated colorinMin/Max.  Changed old
//   min/max to scalingMin/max. 
//
//   Brad Whitlock, Fri Oct 26 17:07:55 PST 2001
//   Enabled the wireframe widgets.
//
//   Brad Whitlock, Fri Nov 9 11:31:26 PDT 2001
//   Changed a constant name so it conforms to the name given by xml2atts.
//
//   Kathleen Bonnell, Thu Mar 28 14:03:19 PST 2002 
//   Reflect change in atts ordering.
//
//   Kathleen Bonnell, Wed May 29 13:40:22 PDT 2002  
//   Remove MinMaxEnabled as user setting minmax no longer dependent upon
//   limits mode. 
//
//   Jeremy Meredith, Tue Nov 16 11:39:53 PST 2004
//   Replaced simple QString::sprintf's with a setNum because there seems
//   to be a bug causing numbers to be incremented by .00001.  See '5263.
//
//   Brad Whitlock, Fri Jul 18 11:58:52 PDT 2008
//   Qt 4.
//  
//   Kathleen Bonnell, Mon Jan 17 17:54:48 MST 2011
//   Change colorTableButton to colorTableWidget to gain invert toggle.
//
//   Kathleen Biagas, Thu Apr 9 07:19:54 MST 2015
//   Use helper function DoubleToQString for consistency in formatting across
//   all windows.
//
// ****************************************************************************

void
QvisSurfacePlotWindow::UpdateWindow(bool doAll)
{
    // Loop through all the attributes and do something for
    // each of them that changed. This function is only responsible
    // for displaying the state values and setting widget sensitivity.
    for(int i = 0; i < surfaceAtts->NumAttributes(); ++i)
    {
        if(!doAll)
        {
            if(!surfaceAtts->IsSelected(i))
            continue;
        }

        switch(i)
        {
        case SurfaceAttributes::ID_legendFlag:
            legendToggle->blockSignals(true);
            legendToggle->setChecked(surfaceAtts->GetLegendFlag());
            legendToggle->blockSignals(false);
            break;

        case SurfaceAttributes::ID_lightingFlag:
            lightingToggle->blockSignals(true);
            lightingToggle->setChecked(surfaceAtts->GetLightingFlag());
            lightingToggle->blockSignals(false);
            break;

        case SurfaceAttributes::ID_surfaceFlag:
            surfaceGroup->blockSignals(true);
            surfaceGroup->setChecked(surfaceAtts->GetSurfaceFlag());
            surfaceGroup->blockSignals(false);
            break;

        case SurfaceAttributes::ID_wireframeFlag:
            wireframeGroup->blockSignals(true);
            wireframeGroup->setChecked(surfaceAtts->GetWireframeFlag());
            wireframeGroup->blockSignals(false);
            break;

        case SurfaceAttributes::ID_limitsMode:
            limitsSelect->blockSignals(true);
            limitsSelect->setCurrentIndex(surfaceAtts->GetLimitsMode());
            limitsSelect->blockSignals(false);
            break;

        case SurfaceAttributes::ID_minFlag:
            minToggle->blockSignals(true);
            minToggle->setChecked(surfaceAtts->GetMinFlag());
            minLineEdit->setEnabled(surfaceAtts->GetMinFlag());
            minToggle->blockSignals(false);
            break;

        case SurfaceAttributes::ID_maxFlag:
            maxToggle->blockSignals(true);
            maxToggle->setChecked(surfaceAtts->GetMaxFlag());
            maxLineEdit->setEnabled(surfaceAtts->GetMaxFlag());
            maxToggle->blockSignals(false);
            break;

        case SurfaceAttributes::ID_colorByZFlag :
            colorModeButtons->button(surfaceAtts->GetColorByZFlag() ? 0:1)->setChecked(true);

            surfaceColor->setEnabled(!surfaceAtts->GetColorByZFlag());
            colorTableWidget->setEnabled(surfaceAtts->GetColorByZFlag());
            break;

        case SurfaceAttributes::ID_scaling:
            scalingButtons->button(surfaceAtts->GetScaling())->setChecked(true);
            skewLineEdit->setEnabled(surfaceAtts->GetScaling() ==
                SurfaceAttributes::Skew);
            break;

        case SurfaceAttributes::ID_lineWidth:
            lineWidth->blockSignals(true);
            lineWidth->SetLineWidth(surfaceAtts->GetLineWidth());
            lineWidth->blockSignals(false);
            break;

        case SurfaceAttributes::ID_surfaceColor:
            { // new scope
            QColor temp(surfaceAtts->GetSurfaceColor().Red(),
                        surfaceAtts->GetSurfaceColor().Green(),
                        surfaceAtts->GetSurfaceColor().Blue());
            surfaceColor->blockSignals(true);
            surfaceColor->setButtonColor(temp);
            surfaceColor->blockSignals(false);
            }
            break;

        case SurfaceAttributes::ID_wireframeColor:
            { // new scope
            QColor temp(surfaceAtts->GetWireframeColor().Red(),
                        surfaceAtts->GetWireframeColor().Green(),
                        surfaceAtts->GetWireframeColor().Blue());
            wireframeColor->blockSignals(true);
            wireframeColor->setButtonColor(temp);
            wireframeColor->blockSignals(false);
            }
            break;

        case SurfaceAttributes::ID_skewFactor:
            skewLineEdit->setText(DoubleToQString(surfaceAtts->GetSkewFactor()));
            break;

        case SurfaceAttributes::ID_min:
            minLineEdit->setText(DoubleToQString(surfaceAtts->GetMin()));
            break;

        case SurfaceAttributes::ID_max:
            maxLineEdit->setText(DoubleToQString(surfaceAtts->GetMax()));
            break;

        case SurfaceAttributes::ID_colorTableName:
            colorTableWidget->setColorTable(
                              surfaceAtts->GetColorTableName().c_str());
            break;
        case SurfaceAttributes::ID_invertColorTable:
            colorTableWidget->setInvertColorTable(
                              surfaceAtts->GetInvertColorTable());
            break;
        }

    } // end for
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::GetCurrentValues
//
// Purpose: 
//   Gets the current values from the text fields and puts the values in the
//   surfaceAtts.
//
// Arguments:
//   which_widget : A number indicating which line edit for which to get
//                  the value. An index of -1 gets them all.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001 
//
// Modifications:
//   Kathleen Bonnell, Thu Oct 11 12:45:30 PDT 2001 
//   Changed old min/max to scalingMin/Max. Added coloringMin/Max.
//
//   Brad Whitlock, Fri Feb 15 11:51:09 PDT 2002
//   Fixed format strings.
//
//   Kathleen Bonnell, Thu Mar 28 14:03:19 PST 2002 
//   Changed back to min/max, removed scalingMin/Max and coloringMin/Max.
//
//   Brad Whitlock, Wed Apr 23 12:03:20 PDT 2008
//   Support for internationalization.
//
//   Brad Whitlock, Fri Jul 18 12:05:14 PDT 2008
//   Qt 4.
//
// ****************************************************************************

void
QvisSurfacePlotWindow::GetCurrentValues(int which_widget)
{
    bool doAll = (which_widget == -1);
    double val;

    // Do the skew factor.
    if(which_widget == SurfaceAttributes::ID_skewFactor || doAll)
    {
        if(LineEditGetDouble(skewLineEdit, val))
            surfaceAtts->SetSkewFactor(val);
        else
        {
            ResettingError("skew factor", 
                DoubleToQString(surfaceAtts->GetSkewFactor()));
            surfaceAtts->SetSkewFactor(surfaceAtts->GetSkewFactor());
        }
    }

    // Do the coloring minimum value.
    if(which_widget == SurfaceAttributes::ID_min || doAll)
    {
        if(LineEditGetDouble(minLineEdit, val))
            surfaceAtts->SetMin(val);
        else
        {
            ResettingError("minimum", 
                DoubleToQString(surfaceAtts->GetMin()));
            surfaceAtts->SetMin(surfaceAtts->GetMin());
        }
    }

    // Do the coloring maximum value.
    if(which_widget == SurfaceAttributes::ID_max || doAll)
    {
        if(LineEditGetDouble(maxLineEdit, val))
            surfaceAtts->SetMax(val);
        else
        {
            ResettingError("maximum", 
                DoubleToQString(surfaceAtts->GetMax()));
            surfaceAtts->SetMax(surfaceAtts->GetMax());
        }
    }
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::Apply
//
// Purpose:
//   This method applies the surface plot attributes and optionally
//   tells the viewer to apply them.
//
// Arguments:
//   ignore : This flag, when true, tells the code to ignore the
//            AutoUpdate function and tell the viewer to apply the
//            surface plot attributes.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//   Eric Brugger, Fri Mar 16 16:17:18 PST 2001
//   I modified the routine to pass to the viewer proxy the plot
//   type stored within the class instead of the one hardwired from
//   an include file.
//
//   Kathleen Bonnell, Tue Apr  3 08:56:47 PDT 2001
//   Removed error checking of var limits for log scaling, error checking
//   now performed at plot level.
// ****************************************************************************

void
QvisSurfacePlotWindow::Apply(bool ignore)
{
    if(AutoUpdate() || ignore)
    {
        // Get the current surface plot attributes and tell the other
        // observers about them.
        GetCurrentValues(-1);

        surfaceAtts->Notify();

        // Tell the viewer to set the surface plot attributes.
        GetViewerMethods()->SetPlotOptions(plotType);
    }
    else
        surfaceAtts->Notify();
}


//
// Qt Slot functions...
//

// ****************************************************************************
// Method: QvisSurfacePlotWindow::apply
//
// Purpose:
//   This is a Qt slot function that is called when the window's Apply
//   button is clicked.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::apply()
{
    Apply(true);
}



// ****************************************************************************
// Method: QvisSurfacePlotWindow::makeDefault
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   "Make default" button is clicked.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//    Eric Brugger, Fri Mar 16 16:17:18 PST 2001
//    I modified the routine to pass to the viewer proxy the plot
//    type stored within the class instead of the one hardwired from
//    an include file.
//
// ****************************************************************************

void
QvisSurfacePlotWindow::makeDefault()
{
    // Tell the viewer to set the default surface plot attributes.
    GetCurrentValues(-1);
    surfaceAtts->Notify();
    GetViewerMethods()->SetDefaultPlotOptions(plotType);
}



// ****************************************************************************
// Method: QvisSurfacePlotWindow::reset
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   Reset button is clicked.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::reset()
{
    // Tell the viewer to reset the surface plot attributes to the last
    // applied values.
    GetViewerMethods()->ResetPlotOptions(plotType);
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::lineWidthChanged
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   line width widget is changed.
//
// Arguments:
//   newWidth : The new line width.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::lineWidthChanged(int newWidth)
{
    surfaceAtts->SetLineWidth(newWidth);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::legendToggled
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   legend toggle button is clicked.
//
// Arguments:
//   val : The new toggle value.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::legendToggled(bool val)
{
    surfaceAtts->SetLegendFlag(val);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::lightingToggled
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   lighting toggle button is clicked.
//
// Arguments:
//   val : The new toggle value.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::lightingToggled(bool val)
{
    surfaceAtts->SetLightingFlag(val);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::scaleClicked
//
// Purpose:
//   This is a Qt slot function that is called when a scale button is clicked.
//
// Arguments:
//   button : Which scaling button was selected
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::scaleClicked(int button)
{
    if (button != surfaceAtts->GetScaling())
    {
        if(button == 1)
            surfaceAtts->SetScaling(SurfaceAttributes::Log);
        else if(button == 2)
            surfaceAtts->SetScaling(SurfaceAttributes::Skew);
        else
            surfaceAtts->SetScaling(SurfaceAttributes::Linear);
        Apply();
    }
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::surfaceToggled
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   surface toggle button is clicked.
//
// Arguments:
//   val : The new toggle value.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::surfaceToggled(bool val)
{
    surfaceAtts->SetSurfaceFlag(val);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::wireframeToggled
//
// Purpose:
//   This is a Qt slot function that is called when the window's
//   wireframe toggle button is clicked.
//
// Arguments:
//   val : The new toggle value.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::wireframeToggled(bool val)
{
    surfaceAtts->SetWireframeFlag(val);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::surfaceColorChanged
//
// Purpose:
//   This is a Qt slot function that is called when the surface color button's
//   color changes.
//
// Arguments:
//   color : The new surface color.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//   Brad Whitlock, Sat Jun 16 18:39:59 PST 2001
//   Added a line of code to set the ColorByZ flag to false if we've selected
//   a new color for the flat-colored surface.
//
// ****************************************************************************

void
QvisSurfacePlotWindow::surfaceColorChanged(const QColor &color)
{
    ColorAttribute temp(color.red(), color.green(), color.blue());
    surfaceAtts->SetSurfaceColor(temp);
    surfaceAtts->SetColorByZFlag(false);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::wireframeColorChanged
//
// Purpose:
//   This is a Qt slot function that is called when the wireframe
//   color button's color changes.
//
// Arguments:
//   color : The new wireframe color.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::wireframeColorChanged(const QColor &color)
{
    ColorAttribute temp(color.red(), color.green(), color.blue());
    surfaceAtts->SetWireframeColor(temp);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::colorModeChanged
//
// Purpose:
//   This is a Qt slot function that is called when a
//   colorBy button is clicked.
//
// Arguments:
//   button  :  Which colorMode button was selected.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::colorModeChanged(int button)
{
    // Only do it if it changed.
    if (0 == button)
        surfaceAtts->SetColorByZFlag(true);
    else
        surfaceAtts->SetColorByZFlag(false);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::processSkewText
//
// Purpose:
//   This is a Qt slot function that is called when the skew factor changes.
//
// Programmer: Kathleen Bonnell
// Creation:   March 06, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::processSkewText()
{
    GetCurrentValues(SurfaceAttributes::ID_skewFactor);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::minToggled
//
// Purpose:
//   This is a Qt slot function that is called when the min toggle is clicked.
//
// Arguments:
//   val : The toggle's new value.
//
// Programmer: Kathleen Bonnell
// Creation:   October 11, 2001 
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::minToggled(bool val)
{
    surfaceAtts->SetMinFlag(val);
    Apply();
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::processMinLimitText
//
// Purpose:
//   This is a Qt slot function that is called when the min limit changes.
//
// Programmer: Kathleen Bonnell
// Creation:   October 11, 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::processMinLimitText()
{
    GetCurrentValues(SurfaceAttributes::ID_min);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::maxToggled
//
// Purpose:
//   This is a Qt slot function that is called when the max toggle is clicked.
//
// Arguments:
//   val : The toggle's new value.
//
// Programmer: Kathleen Bonnell
// Creation:   October 11, 2001 
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::maxToggled(bool val)
{
    surfaceAtts->SetMaxFlag(val);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::processMaxLimitText
//
// Purpose:
//   This is a Qt slot function that is called when the max limit changes.
//
// Programmer: Kathleen Bonnell
// Creation:   October 11, 2001 
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::processMaxLimitText()
{
    GetCurrentValues(SurfaceAttributes::ID_max);
    Apply();
}


// ****************************************************************************
// Method: QvisSurfacePlotWindow::colorTableClicked
//
// Purpose:
//   This is a Qt slot function that sets the desired color table name into
//   the surface plot attributes.
//
// Arguments:
//   <unnamed>  : If this is true, we want to use the default color table.
//   ctName     : The name of the color table to use if we're not going to
//                use the default.
//
// Programmer: Brad Whitlock
// Creation:   Sat Jun 16 18:30:51 PST 2001
//
// Modifications:
//
// ****************************************************************************

void
QvisSurfacePlotWindow::colorTableClicked(bool , const QString &ctName)
{
    surfaceAtts->SetColorByZFlag(true);
    surfaceAtts->SetColorTableName(ctName.toStdString());
    Apply();
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::invertColorTableToggled
//
// Purpose: 
//   This is a Qt slot function that sets the invert color table flag into the
//   surface plot attributes.
//
// Arguments:
//   val    :  Whether or not to invert the color table.
//
// Programmer: Kathleen Bonnell
// Creation:   January  17, 2011
//
// Modifications:
//   
// ****************************************************************************

void
QvisSurfacePlotWindow::invertColorTableToggled(bool val)
{
    surfaceAtts->SetInvertColorTable(val);
    Apply();
}

// ****************************************************************************
// Method: QvisSurfacePlotWindow::limitsSelectChanged
//
// Purpose:
//   This is a Qt slot function that sets 
//
// Arguments:
//   limitsMode : Which type of limits should be used for coloring. 
//              
//
// Programmer: Kathleen Bonnell 
// Creation:   October 11, 2001
//
// ****************************************************************************

void
QvisSurfacePlotWindow::limitsSelectChanged(int limitsMode)
{
    if (surfaceAtts->GetLimitsMode() != limitsMode)
    {
        if(limitsMode == 0)
            surfaceAtts->SetLimitsMode(SurfaceAttributes::OriginalData);
        else
            surfaceAtts->SetLimitsMode(SurfaceAttributes::CurrentPlot);
        Apply();
    }
}

