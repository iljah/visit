package llnl.visit.plots;

import llnl.visit.AttributeSubject;
import llnl.visit.CommunicationBuffer;
import llnl.visit.Plugin;
import llnl.visit.ColorAttribute;

// ****************************************************************************
// Class: MeshAttributes
//
// Purpose:
//    Attributes for the mesh plot
//
// Notes:      Autogenerated by xml2java.
//
// Programmer: xml2java
// Creation:   Thu Jul 31 16:07:58 PST 2003
//
// Modifications:
//   
// ****************************************************************************

public class MeshAttributes extends AttributeSubject implements Plugin
{
    public MeshAttributes()
    {
        super(14);

        legendFlag = true;
        lineStyle = 0;
        lineWidth = 0;
        meshColor = new ColorAttribute(0, 0, 0);
        outlineOnlyFlag = false;
        errorTolerance = 0.01;
        opaqueFlag = false;
        pointSize = 0.05;
        opaqueColor = new ColorAttribute(255, 255, 255);
        backgroundFlag = true;
        foregroundFlag = true;
        smoothingLevel = 0;
        pointSizeVarEnabled = false;
        pointSizeVar = new String("default");
    }

    public MeshAttributes(MeshAttributes obj)
    {
        super(14);

        legendFlag = obj.legendFlag;
        lineStyle = obj.lineStyle;
        lineWidth = obj.lineWidth;
        meshColor = new ColorAttribute(obj.meshColor);
        outlineOnlyFlag = obj.outlineOnlyFlag;
        errorTolerance = obj.errorTolerance;
        opaqueFlag = obj.opaqueFlag;
        pointSize = obj.pointSize;
        opaqueColor = new ColorAttribute(obj.opaqueColor);
        backgroundFlag = obj.backgroundFlag;
        foregroundFlag = obj.foregroundFlag;
        smoothingLevel = obj.smoothingLevel;
        pointSizeVarEnabled = obj.pointSizeVarEnabled;
        pointSizeVar = new String(obj.pointSizeVar);

        SelectAll();
    }

    public boolean equals(MeshAttributes obj)
    {
        // Create the return value
        return ((legendFlag == obj.legendFlag) &&
                (lineStyle == obj.lineStyle) &&
                (lineWidth == obj.lineWidth) &&
                (meshColor == obj.meshColor) &&
                (outlineOnlyFlag == obj.outlineOnlyFlag) &&
                (errorTolerance == obj.errorTolerance) &&
                (opaqueFlag == obj.opaqueFlag) &&
                (pointSize == obj.pointSize) &&
                (opaqueColor == obj.opaqueColor) &&
                (backgroundFlag == obj.backgroundFlag) &&
                (foregroundFlag == obj.foregroundFlag) &&
                (smoothingLevel == obj.smoothingLevel) &&
                (pointSizeVarEnabled == obj.pointSizeVarEnabled) &&
                (pointSizeVar == obj.pointSizeVar));
    }

    public String GetName() { return "Mesh"; }
    public String GetVersion() { return "1.0"; }

    // Property setting methods
    public void SetLegendFlag(boolean legendFlag_)
    {
        legendFlag = legendFlag_;
        Select(0);
    }

    public void SetLineStyle(int lineStyle_)
    {
        lineStyle = lineStyle_;
        Select(1);
    }

    public void SetLineWidth(int lineWidth_)
    {
        lineWidth = lineWidth_;
        Select(2);
    }

    public void SetMeshColor(ColorAttribute meshColor_)
    {
        meshColor = meshColor_;
        Select(3);
    }

    public void SetOutlineOnlyFlag(boolean outlineOnlyFlag_)
    {
        outlineOnlyFlag = outlineOnlyFlag_;
        Select(4);
    }

    public void SetErrorTolerance(double errorTolerance_)
    {
        errorTolerance = errorTolerance_;
        Select(5);
    }

    public void SetOpaqueFlag(boolean opaqueFlag_)
    {
        opaqueFlag = opaqueFlag_;
        Select(6);
    }

    public void SetPointSize(double pointSize_)
    {
        pointSize = pointSize_;
        Select(7);
    }

    public void SetOpaqueColor(ColorAttribute opaqueColor_)
    {
        opaqueColor = opaqueColor_;
        Select(8);
    }

    public void SetBackgroundFlag(boolean backgroundFlag_)
    {
        backgroundFlag = backgroundFlag_;
        Select(9);
    }

    public void SetForegroundFlag(boolean foregroundFlag_)
    {
        foregroundFlag = foregroundFlag_;
        Select(10);
    }

    public void SetSmoothingLevel(int smoothingLevel_)
    {
        smoothingLevel = smoothingLevel_;
        Select(11);
    }

    public void SetPointSizeVarEnabled(boolean pointSizeVarEnabled_)
    {
        pointSizeVarEnabled = pointSizeVarEnabled_;
        Select(12);
    }

    public void SetPointSizeVar(String pointSizeVar_)
    {
        pointSizeVar = pointSizeVar_;
        Select(13);
    }

    // Property getting methods
    public boolean        GetLegendFlag() { return legendFlag; }
    public int            GetLineStyle() { return lineStyle; }
    public int            GetLineWidth() { return lineWidth; }
    public ColorAttribute GetMeshColor() { return meshColor; }
    public boolean        GetOutlineOnlyFlag() { return outlineOnlyFlag; }
    public double         GetErrorTolerance() { return errorTolerance; }
    public boolean        GetOpaqueFlag() { return opaqueFlag; }
    public double         GetPointSize() { return pointSize; }
    public ColorAttribute GetOpaqueColor() { return opaqueColor; }
    public boolean        GetBackgroundFlag() { return backgroundFlag; }
    public boolean        GetForegroundFlag() { return foregroundFlag; }
    public int            GetSmoothingLevel() { return smoothingLevel; }
    public boolean        GetPointSizeVarEnabled() { return pointSizeVarEnabled; }
    public String         GetPointSizeVar() { return pointSizeVar; }

    // Write and read methods.
    public void WriteAtts(CommunicationBuffer buf)
    {
        if(WriteSelect(0, buf))
            buf.WriteBool(legendFlag);
        if(WriteSelect(1, buf))
            buf.WriteInt(lineStyle);
        if(WriteSelect(2, buf))
            buf.WriteInt(lineWidth);
        if(WriteSelect(3, buf))
            meshColor.Write(buf);
        if(WriteSelect(4, buf))
            buf.WriteBool(outlineOnlyFlag);
        if(WriteSelect(5, buf))
            buf.WriteDouble(errorTolerance);
        if(WriteSelect(6, buf))
            buf.WriteBool(opaqueFlag);
        if(WriteSelect(7, buf))
            buf.WriteDouble(pointSize);
        if(WriteSelect(8, buf))
            opaqueColor.Write(buf);
        if(WriteSelect(9, buf))
            buf.WriteBool(backgroundFlag);
        if(WriteSelect(10, buf))
            buf.WriteBool(foregroundFlag);
        if(WriteSelect(11, buf))
            buf.WriteInt(smoothingLevel);
        if(WriteSelect(12, buf))
            buf.WriteBool(pointSizeVarEnabled);
        if(WriteSelect(13, buf))
            buf.WriteString(pointSizeVar);
    }

    public void ReadAtts(int n, CommunicationBuffer buf)
    {
        for(int i = 0; i < n; ++i)
        {
            int index = (int)buf.ReadByte();
            switch(index)
            {
            case 0:
                SetLegendFlag(buf.ReadBool());
                break;
            case 1:
                SetLineStyle(buf.ReadInt());
                break;
            case 2:
                SetLineWidth(buf.ReadInt());
                break;
            case 3:
                meshColor.Read(buf);
                Select(3);
                break;
            case 4:
                SetOutlineOnlyFlag(buf.ReadBool());
                break;
            case 5:
                SetErrorTolerance(buf.ReadDouble());
                break;
            case 6:
                SetOpaqueFlag(buf.ReadBool());
                break;
            case 7:
                SetPointSize(buf.ReadDouble());
                break;
            case 8:
                opaqueColor.Read(buf);
                Select(8);
                break;
            case 9:
                SetBackgroundFlag(buf.ReadBool());
                break;
            case 10:
                SetForegroundFlag(buf.ReadBool());
                break;
            case 11:
                SetSmoothingLevel(buf.ReadInt());
                break;
            case 12:
                SetPointSizeVarEnabled(buf.ReadBool());
                break;
            case 13:
                SetPointSizeVar(buf.ReadString());
                break;
            }
        }
    }


    // Attributes
    private boolean        legendFlag;
    private int            lineStyle;
    private int            lineWidth;
    private ColorAttribute meshColor;
    private boolean        outlineOnlyFlag;
    private double         errorTolerance;
    private boolean        opaqueFlag;
    private double         pointSize;
    private ColorAttribute opaqueColor;
    private boolean        backgroundFlag;
    private boolean        foregroundFlag;
    private int            smoothingLevel;
    private boolean        pointSizeVarEnabled;
    private String         pointSizeVar;
}

