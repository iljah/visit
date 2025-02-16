// Copyright (c) Lawrence Livermore National Security, LLC and other VisIt
// Project developers.  See the top-level LICENSE file for dates and other
// details.  No copyright assignment is required to contribute to VisIt.

#include <PyText2DObject.h>
#include <ObserverToCallback.h>
#include <ColorAttribute.h>

// CUSTOM:
#include <Py2and3Support.h>

// Functions that we need in visitmodule.C
extern void UpdateAnnotationHelper(AnnotationObject *);
extern bool DeleteAnnotationObjectHelper(AnnotationObject *);

// ****************************************************************************
// Module: PyText2DObject
//
// Purpose:
//   This class is a Python wrapper arround AnnotationObject that makes it
//   look like a Text2D object.
//
// Note:       Autogenerated by xml2python. Do not modify by hand!
//
// Programmer: xml2python
// Creation:   Thu Dec 4 09:53:48 PDT 2003
//
// ****************************************************************************

//
// This struct contains the Python type information and a Text2DObject.
//
struct Text2DObjectObject
{
    PyObject_HEAD
    AnnotationObject *data;
    bool owns;
};

//
// Internal prototypes
//
static PyObject *NewText2DObject();

static PyObject *
Text2DObject_SetVisible(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the visible in the object.
    obj->data->SetVisible(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetVisible(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetVisible()?1L:0L);
    return retval;
}

static PyObject *
Text2DObject_SetActive(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the active in the object.
    obj->data->SetActive(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetActive(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetActive()?1L:0L);
    return retval;
}

static PyObject *
Text2DObject_SetPosition(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    double *dvals = obj->data->GetPosition();
    if(!PyArg_ParseTuple(args, "dd", &dvals[0], &dvals[1]))
    {
        PyObject     *tuple;
        if(!PyArg_ParseTuple(args, "O", &tuple))
            return NULL;

        if(PyTuple_Check(tuple))
        {
            if(PyTuple_Size(tuple) != 2)
                return NULL;

            PyErr_Clear();
            for(int i = 0; i < PyTuple_Size(tuple); ++i)
            {
                PyObject *item = PyTuple_GET_ITEM(tuple, i);
                if(PyFloat_Check(item))
                    dvals[i] = (PyFloat_AS_DOUBLE(item));
                else if(PyInt_Check(item))
                    dvals[i] = double(PyInt_AS_LONG(item));
                else if(PyLong_Check(item))
                    dvals[i] = (PyLong_AsDouble(item));
                else
                    dvals[i] = 0.;
            }
        }
        else
            return NULL;
    }

    // Mark the position in the object as modified.
    obj->data->SelectPosition();
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetPosition(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    // Allocate a tuple the with enough entries to hold the position.
    PyObject *retval = PyTuple_New(2);
    const double *position = obj->data->GetPosition();
    for(int i = 0; i < 2; ++i)
        PyTuple_SET_ITEM(retval, i, PyFloat_FromDouble(double(position[i])));
    return retval;
}

static PyObject *
Text2DObject_SetHeight(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    double dval;
    if(!PyArg_ParseTuple(args, "d", &dval))
        return NULL;

    // Set the height in the object.
/*CUSTOM*/
    double *pos2 = obj->data->GetPosition2();
    pos2[0] = dval;
    obj->data->SelectPosition2();
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetHeight(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
/*CUSTOM*/
    double *pos2 = obj->data->GetPosition2();
    PyObject *retval = PyFloat_FromDouble(double(pos2[0]));
    return retval;
}

static PyObject *
Text2DObject_SetTextColor(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int c[4];
    if(!PyArg_ParseTuple(args, "iiii", &c[0], &c[1], &c[2], &c[3]))
    {
        c[3] = 255;
        if(!PyArg_ParseTuple(args, "iii", &c[0], &c[1], &c[2]))
        {
            double dr, dg, db, da;
            if(PyArg_ParseTuple(args, "dddd", &dr, &dg, &db, &da))
            {
                c[0] = int(dr);
                c[1] = int(dg);
                c[2] = int(db);
                c[3] = int(da);
            }
            else if(PyArg_ParseTuple(args, "ddd", &dr, &dg, &db))
            {
                c[0] = int(dr);
                c[1] = int(dg);
                c[2] = int(db);
                c[3] = 255;
            }
            else
            {
                PyObject *tuple = NULL;
                if(!PyArg_ParseTuple(args, "O", &tuple))
                    return NULL;

                if(!PyTuple_Check(tuple))
                    return NULL;

                // Make sure that the tuple is the right size.
                if(PyTuple_Size(tuple) < 3 || PyTuple_Size(tuple) > 4)
                    return NULL;

                // Make sure that all elements in the tuple are ints.
                for(int i = 0; i < PyTuple_Size(tuple); ++i)
                {
                    PyObject *item = PyTuple_GET_ITEM(tuple, i);
                    if(PyInt_Check(item))
                        c[i] = int(PyInt_AS_LONG(PyTuple_GET_ITEM(tuple, i)));
                    else if(PyFloat_Check(item))
                        c[i] = int(PyFloat_AS_DOUBLE(PyTuple_GET_ITEM(tuple, i)));
                    else
                        return NULL;
                }
            }
        }
        PyErr_Clear();
    }

    // Set the textColor in the object.
    ColorAttribute ca(c[0], c[1], c[2], c[3]);
    obj->data->SetTextColor(ca);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetTextColor(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    // Allocate a tuple the with enough entries to hold the textColor.
    PyObject *retval = PyTuple_New(4);
    const unsigned char *textColor = obj->data->GetTextColor().GetColor();
    PyTuple_SET_ITEM(retval, 0, PyInt_FromLong(long(textColor[0])));
    PyTuple_SET_ITEM(retval, 1, PyInt_FromLong(long(textColor[1])));
    PyTuple_SET_ITEM(retval, 2, PyInt_FromLong(long(textColor[2])));
    PyTuple_SET_ITEM(retval, 3, PyInt_FromLong(long(textColor[3])));
    return retval;
}

static PyObject *
Text2DObject_SetUseForegroundForTextColor(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the useForegroundForTextColor in the object.
    obj->data->SetUseForegroundForTextColor(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetUseForegroundForTextColor(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetUseForegroundForTextColor()?1L:0L);
    return retval;
}

static PyObject *
Text2DObject_SetText(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    char *str;
    if(!PyArg_ParseTuple(args, "s", &str))
        return NULL;

    // Set the text in the object.
/*CUSTOM*/
    stringVector s; s.push_back(str);
    obj->data->SetText(s);
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetText(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
/*CUSTOM*/
    const stringVector &s = obj->data->GetText();
    PyObject *retval = PyString_FromString(s.size() > 0 ? s[0].c_str(): "");
    return retval;
}

static PyObject *
Text2DObject_SetFontFamily(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the fontFamily in the object.
    if(ival >= 0 && ival < 3)
    {
/*CUSTOM*/
        obj->data->SetFontFamily(AnnotationObject::FontFamily(ival));
        UpdateAnnotationHelper(obj->data);
    }
    else
    {
        fprintf(stderr, "An invalid fontFamily value was given. "
                        "Valid values are in the range of [0,2]. "
                        "You can also use the following names: "
                        "Arial, Courier, Times.");
        return NULL;
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetFontFamily(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(long(obj->data->GetFontFamily()));
    return retval;
}

static PyObject *
Text2DObject_SetFontBold(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the fontBold in the object.
    obj->data->SetFontBold(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetFontBold(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetFontBold()?1L:0L);
    return retval;
}

static PyObject *
Text2DObject_SetFontItalic(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the fontItalic in the object.
    obj->data->SetFontItalic(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetFontItalic(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetFontItalic()?1L:0L);
    return retval;
}

static PyObject *
Text2DObject_SetFontShadow(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    int ival;
    if(!PyArg_ParseTuple(args, "i", &ival))
        return NULL;

    // Set the fontShadow in the object.
    obj->data->SetFontShadow(ival != 0);
/*CUSTOM*/
    UpdateAnnotationHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject *
Text2DObject_GetFontShadow(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;
    PyObject *retval = PyInt_FromLong(obj->data->GetFontShadow()?1L:0L);
    return retval;
}

/*CUSTOM*/
static PyObject *
Text2DObject_Delete(PyObject *self, PyObject *args)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)self;

    // Tell the VisIt module to decrement the reference count for the
    // AnnotationObject pointed to by obj->data. If there are no more
    // references, this object will own it.
    obj->owns = DeleteAnnotationObjectHelper(obj->data);

    Py_INCREF(Py_None);
    return Py_None;
}

#define TEXT2DOBJECT_NMETH 24
PyMethodDef Text2DObject_methods[TEXT2DOBJECT_NMETH] = {
    {"SetVisible", Text2DObject_SetVisible, METH_VARARGS},
    {"GetVisible", Text2DObject_GetVisible, METH_VARARGS},
    {"SetActive", Text2DObject_SetActive, METH_VARARGS},
    {"GetActive", Text2DObject_GetActive, METH_VARARGS},
    {"SetPosition", Text2DObject_SetPosition, METH_VARARGS},
    {"GetPosition", Text2DObject_GetPosition, METH_VARARGS},
    {"SetHeight", Text2DObject_SetHeight, METH_VARARGS},
    {"GetHeight", Text2DObject_GetHeight, METH_VARARGS},
    {"SetTextColor", Text2DObject_SetTextColor, METH_VARARGS},
    {"GetTextColor", Text2DObject_GetTextColor, METH_VARARGS},
    {"SetUseForegroundForTextColor", Text2DObject_SetUseForegroundForTextColor, METH_VARARGS},
    {"GetUseForegroundForTextColor", Text2DObject_GetUseForegroundForTextColor, METH_VARARGS},
    {"SetText", Text2DObject_SetText, METH_VARARGS},
    {"GetText", Text2DObject_GetText, METH_VARARGS},
    {"SetFontFamily", Text2DObject_SetFontFamily, METH_VARARGS},
    {"GetFontFamily", Text2DObject_GetFontFamily, METH_VARARGS},
    {"SetFontBold", Text2DObject_SetFontBold, METH_VARARGS},
    {"GetFontBold", Text2DObject_GetFontBold, METH_VARARGS},
    {"SetFontItalic", Text2DObject_SetFontItalic, METH_VARARGS},
    {"GetFontItalic", Text2DObject_GetFontItalic, METH_VARARGS},
    {"SetFontShadow", Text2DObject_SetFontShadow, METH_VARARGS},
    {"GetFontShadow", Text2DObject_GetFontShadow, METH_VARARGS},
    {"Delete", Text2DObject_Delete, METH_VARARGS},
    {NULL, NULL}
};

//
// Type functions
//

static void
Text2DObject_dealloc(PyObject *v)
{
   Text2DObjectObject *obj = (Text2DObjectObject *)v;
   if(obj->owns)
       delete obj->data;
}

// OLD
// static int
// Text2DObject_compare(PyObject *v, PyObject *w)
// {
//     AnnotationObject *a = ((Text2DObjectObject *)v)->data;
//     AnnotationObject *b = ((Text2DObjectObject *)w)->data;
//     return (*a == *b) ? 0 : -1;
// }

static PyObject *
Text2DObject_getattr(PyObject *self, char *name)
{
    if(strcmp(name, "visible") == 0)
        return Text2DObject_GetVisible(self, NULL);
    if(strcmp(name, "active") == 0)
        return Text2DObject_GetActive(self, NULL);
    if(strcmp(name, "position") == 0)
        return Text2DObject_GetPosition(self, NULL);
    if(strcmp(name, "height") == 0)
        return Text2DObject_GetHeight(self, NULL);
    if(strcmp(name, "textColor") == 0)
        return Text2DObject_GetTextColor(self, NULL);
    if(strcmp(name, "useForegroundForTextColor") == 0)
        return Text2DObject_GetUseForegroundForTextColor(self, NULL);
    if(strcmp(name, "text") == 0)
        return Text2DObject_GetText(self, NULL);
    if(strcmp(name, "fontFamily") == 0)
        return Text2DObject_GetFontFamily(self, NULL);
    if(strcmp(name, "Arial") == 0)
        return PyInt_FromLong(long(AnnotationObject::Arial));
    else if(strcmp(name, "Courier") == 0)
        return PyInt_FromLong(long(AnnotationObject::Courier));
    else if(strcmp(name, "Times") == 0)
        return PyInt_FromLong(long(AnnotationObject::Times));

    if(strcmp(name, "fontBold") == 0)
        return Text2DObject_GetFontBold(self, NULL);
    if(strcmp(name, "fontItalic") == 0)
        return Text2DObject_GetFontItalic(self, NULL);
    if(strcmp(name, "fontShadow") == 0)
        return Text2DObject_GetFontShadow(self, NULL);

    return Py_FindMethod(Text2DObject_methods, self, name);
}

static int
Text2DObject_setattr(PyObject *self, char *name, PyObject *args)
{
    // Create a tuple to contain the arguments since all of the Set
    // functions expect a tuple.
    PyObject *tuple = PyTuple_New(1);
    PyTuple_SET_ITEM(tuple, 0, args);
    Py_INCREF(args);
    bool retval = false;

    if(strcmp(name, "visible") == 0)
        retval = (Text2DObject_SetVisible(self, tuple) != NULL);
    else if(strcmp(name, "active") == 0)
        retval = (Text2DObject_SetActive(self, tuple) != NULL);
    else if(strcmp(name, "position") == 0)
        retval = (Text2DObject_SetPosition(self, tuple) != NULL);
    else if(strcmp(name, "height") == 0)
        retval = (Text2DObject_SetHeight(self, tuple) != NULL);
    else if(strcmp(name, "textColor") == 0)
        retval = (Text2DObject_SetTextColor(self, tuple) != NULL);
    else if(strcmp(name, "useForegroundForTextColor") == 0)
        retval = (Text2DObject_SetUseForegroundForTextColor(self, tuple) != NULL);
    else if(strcmp(name, "text") == 0)
        retval = (Text2DObject_SetText(self, tuple) != NULL);
    else if(strcmp(name, "fontFamily") == 0)
        retval = (Text2DObject_SetFontFamily(self, tuple) != NULL);
    else if(strcmp(name, "fontBold") == 0)
        retval = (Text2DObject_SetFontBold(self, tuple) != NULL);
    else if(strcmp(name, "fontItalic") == 0)
        retval = (Text2DObject_SetFontItalic(self, tuple) != NULL);
    else if(strcmp(name, "fontShadow") == 0)
        retval = (Text2DObject_SetFontShadow(self, tuple) != NULL);

    Py_DECREF(tuple);
    return retval ? 0 : -1;
}

static int
Text2DObject_print(PyObject *v, FILE *fp, int flags)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)v;

    if(obj->data->GetVisible())
        fprintf(fp, "visible = 1\n");
    else
        fprintf(fp, "visible = 0\n");
    if(obj->data->GetActive())
        fprintf(fp, "active = 1\n");
    else
        fprintf(fp, "active = 0\n");
/*CUSTOM*/
    {   const double *position = obj->data->GetPosition();
        fprintf(fp, "position = (%g, %g)\n", position[0], position[1]);
    }
    const double *position2 = obj->data->GetPosition2();
    fprintf(fp, "height = %g\n", position2[0]);
    const unsigned char *textColor = obj->data->GetTextColor().GetColor();
    fprintf(fp, "textColor = (%d, %d, %d, %d)\n", int(textColor[0]), int(textColor[1]), int(textColor[2]), int(textColor[3]));
    if(obj->data->GetUseForegroundForTextColor())
        fprintf(fp, "useForegroundForTextColor = 1\n");
    else
        fprintf(fp, "useForegroundForTextColor = 0\n");
    const stringVector &s = obj->data->GetText();
    fprintf(fp, "text = \"%s\"\n", s.size() > 0 ? s[0].c_str() : "");
    const char *fontFamily_names = "Arial, Courier, Times";
    if(obj->data->GetFontFamily() == AnnotationObject::Arial)
        fprintf(fp, "fontFamily = Arial  # %s\n", fontFamily_names);
    else if(obj->data->GetFontFamily() == AnnotationObject::Courier)
        fprintf(fp, "fontFamily = Courier  # %s\n", fontFamily_names);
    else
        fprintf(fp, "fontFamily = Times  # %s\n", fontFamily_names);

    if(obj->data->GetFontBold())
        fprintf(fp, "fontBold = 1\n");
    else
        fprintf(fp, "fontBold = 0\n");
    if(obj->data->GetFontItalic())
        fprintf(fp, "fontItalic = 1\n");
    else
        fprintf(fp, "fontItalic = 0\n");
    if(obj->data->GetFontShadow())
        fprintf(fp, "fontShadow = 1\n");
    else
        fprintf(fp, "fontShadow = 0\n");

    return 0;
}

static PyObject *
PyText2DObject_StringRepresentation(const AnnotationObject *atts)
{
    std::string str;
    char tmpStr[1000];

    if(atts->GetVisible())
        snprintf(tmpStr, 1000, "visible = 1\n");
    else
        snprintf(tmpStr, 1000, "visible = 0\n");
    str += tmpStr;
    if(atts->GetActive())
        snprintf(tmpStr, 1000, "active = 1\n");
    else
        snprintf(tmpStr, 1000, "active = 0\n");
/*CUSTOM*/
    {   const double *position = atts->GetPosition();
        snprintf(tmpStr, 1000, "position = (%g, %g)\n", position[0], position[1]);
    }
    str += tmpStr;
    const double *position2 = atts->GetPosition2();
    snprintf(tmpStr, 1000, "height = %g\n", position2[0]);
    str += tmpStr;
    const unsigned char *textColor = atts->GetTextColor().GetColor();
    snprintf(tmpStr, 1000, "textColor = (%d, %d, %d, %d)\n", int(textColor[0]), int(textColor[1]), int(textColor[2]), int(textColor[3]));
    str += tmpStr;
    if(atts->GetUseForegroundForTextColor())
        snprintf(tmpStr, 1000, "useForegroundForTextColor = 1\n");
    else
        snprintf(tmpStr, 1000, "useForegroundForTextColor = 0\n");
    str += tmpStr;
    const stringVector &s = atts->GetText();
    snprintf(tmpStr, 1000, "text = \"%s\"\n", s.size() > 0 ? s[0].c_str() : "");
    str += tmpStr;
    const char *fontFamily_names = "Arial, Courier, Times";
    if(atts->GetFontFamily() == AnnotationObject::Arial)
        snprintf(tmpStr, 1000, "fontFamily = Arial  # %s\n", fontFamily_names);
    else if(atts->GetFontFamily() == AnnotationObject::Courier)
        snprintf(tmpStr, 1000, "fontFamily = Courier  # %s\n", fontFamily_names);
    else
        snprintf(tmpStr, 1000, "fontFamily = Times  # %s\n", fontFamily_names);
    str += tmpStr;

    if(atts->GetFontBold())
        snprintf(tmpStr, 1000, "fontBold = 1\n");
    else
        snprintf(tmpStr, 1000, "fontBold = 0\n");
    str += tmpStr;
    if(atts->GetFontItalic())
        snprintf(tmpStr, 1000, "fontItalic = 1\n");
    else
        snprintf(tmpStr, 1000, "fontItalic = 0\n");
    str += tmpStr;
    if(atts->GetFontShadow())
        snprintf(tmpStr, 1000, "fontShadow = 1\n");
    else
        snprintf(tmpStr, 1000, "fontShadow = 0\n");
    str += tmpStr;
    return PyString_FromString(str.c_str());
}

static PyObject *
Text2DObject_str(PyObject *v)
{
    Text2DObjectObject *obj = (Text2DObjectObject *)v;
    return PyText2DObject_StringRepresentation(obj->data);
}

//
// The doc string for the class.
//
#if PY_MAJOR_VERSION > 2 || (PY_MAJOR_VERSION == 2 && PY_MINOR_VERSION >= 5)
static const char *Text2DObject_Purpose = "This class defines defines an interface to a 2D text object.";
#else
static char *Text2DObject_Purpose = "This class defines defines an interface to a 2D text object.";
#endif

// CUSTOM
static PyObject *Text2DObject_richcompare(PyObject *self, PyObject *other, int op);

// CUSTOM

//
// Python Type Struct Def Macro from Py2and3Support.h
//
//         VISIT_PY_TYPE_OBJ( VPY_TYPE,
//                            VPY_NAME,
//                            VPY_OBJECT,
//                            VPY_DEALLOC,
//                            VPY_PRINT,
//                            VPY_GETATTR,
//                            VPY_SETATTR,
//                            VPY_STR,
//                            VPY_PURPOSE,
//                            VPY_RICHCOMP,
//                            VPY_AS_NUMBER)

//
// The type description structure
//
VISIT_PY_TYPE_OBJ( Text2DObjectType,         \
                   "Text2DObject",           \
                   Text2DObjectObject,       \
                   Text2DObject_dealloc,     \
                   Text2DObject_print,       \
                   Text2DObject_getattr,     \
                   Text2DObject_setattr,     \
                   Text2DObject_str,         \
                   Text2DObject_Purpose,     \
                   Text2DObject_richcompare, \
                   0); /* as_number*/

// CUSTOM
static PyObject *
Text2DObject_richcompare(PyObject *self, PyObject *other, int op)
{
    // only compare against the same type
    if ( Py_TYPE(self) != Py_TYPE(other)
         || Py_TYPE(self) != &Text2DObjectType)
    {
        Py_INCREF(Py_NotImplemented);
        return Py_NotImplemented;
    }

    PyObject *res = NULL;
    AnnotationObject *a = ((Text2DObjectObject *)self)->data;
    AnnotationObject *b = ((Text2DObjectObject *)other)->data;

    switch (op)
    {
       case Py_EQ:
           res = (*a == *b) ? Py_True : Py_False;
           break;
       case Py_NE:
           res = (*a != *b) ? Py_True : Py_False;
           break;
       default:
           res = Py_NotImplemented;
           break;
    }

    Py_INCREF(res);
    return res;
}


//
// Helper functions for object allocation.
//

static PyObject *
NewText2DObject()
{
    Text2DObjectObject *newObject;
    newObject = PyObject_NEW(Text2DObjectObject, &Text2DObjectType);
    if(newObject == NULL)
        return NULL;
    newObject->data = new AnnotationObject;
    newObject->data->SetObjectType(AnnotationObject::Text2D);
    newObject->owns = true;
    return (PyObject *)newObject;
}

static PyObject *
WrapText2DObject(AnnotationObject *annot)
{
    Text2DObjectObject *newObject;
    newObject = PyObject_NEW(Text2DObjectObject, &Text2DObjectType);
    if(newObject == NULL)
        return NULL;
    newObject->data = annot;
    newObject->owns = false;
    return (PyObject *)newObject;
}

///////////////////////////////////////////////////////////////////////////////
//
// Interface that is exposed to the VisIt module.
//
///////////////////////////////////////////////////////////////////////////////

bool
PyText2DObject_Check(PyObject *obj)
{
    return (obj->ob_type == &Text2DObjectType);
}

AnnotationObject *
PyText2DObject_FromPyObject(PyObject *obj)
{
    Text2DObjectObject *obj2 = (Text2DObjectObject *)obj;
    return obj2->data;
}

PyObject *
PyText2DObject_NewPyObject()
{
    return NewText2DObject();
}

PyObject *
PyText2DObject_WrapPyObject(AnnotationObject *attr)
{
    return WrapText2DObject(attr);
}

