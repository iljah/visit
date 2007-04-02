#ifndef QUERYLIST_H
#define QUERYLIST_H
#include <state_exports.h>
#include <string>
#include <AttributeSubject.h>

// ****************************************************************************
// Class: QueryList
//
// Purpose:
//    List of supported queries
//
// Notes:      Autogenerated by xml2atts.
//
// Programmer: xml2atts
// Creation:   Wed Nov 9 09:18:11 PDT 2005
//
// Modifications:
//   
// ****************************************************************************

class STATE_API QueryList : public AttributeSubject
{
public:
    enum QueryType
    {
        DatabaseQuery,
        PointQuery,
        LineQuery
    };
    enum WindowType
    {
        Basic,
        SinglePoint,
        DoublePoint,
        DomainNode,
        DomainNodeVars,
        DomainZone,
        DomainZoneVars,
        ActualData,
        ActualDataVars
    };
    enum Groups
    {
        CurveRelated,
        MeshRelated,
        PickRelated,
        TimeRelated,
        VariableRelated
    };
    enum QueryMode
    {
        QueryOnly,
        QueryAndTime,
        TimeOnly
    };

    QueryList();
    QueryList(const QueryList &obj);
    virtual ~QueryList();

    virtual QueryList& operator = (const QueryList &obj);
    virtual bool operator == (const QueryList &obj) const;
    virtual bool operator != (const QueryList &obj) const;

    virtual const std::string TypeName() const;
    virtual bool CopyAttributes(const AttributeGroup *);
    virtual AttributeSubject *CreateCompatible(const std::string &) const;
    virtual AttributeSubject *NewInstance(bool) const;

    // Property selection methods
    virtual void SelectAll();
    void SelectNames();
    void SelectTypes();
    void SelectGroups();
    void SelectNumInputs();
    void SelectAllowedVarTypes();
    void SelectWinType();
    void SelectQueryMode();
    void SelectNumVars();

    // Property setting methods
    void SetNames(const stringVector &names_);
    void SetTypes(const intVector &types_);
    void SetGroups(const intVector &groups_);
    void SetNumInputs(const intVector &numInputs_);
    void SetAllowedVarTypes(const intVector &allowedVarTypes_);
    void SetWinType(const intVector &winType_);
    void SetQueryMode(const intVector &queryMode_);
    void SetNumVars(const intVector &numVars_);

    // Property getting methods
    const stringVector &GetNames() const;
          stringVector &GetNames();
    const intVector    &GetTypes() const;
          intVector    &GetTypes();
    const intVector    &GetGroups() const;
          intVector    &GetGroups();
    const intVector    &GetNumInputs() const;
          intVector    &GetNumInputs();
    const intVector    &GetAllowedVarTypes() const;
          intVector    &GetAllowedVarTypes();
    const intVector    &GetWinType() const;
          intVector    &GetWinType();
    const intVector    &GetQueryMode() const;
          intVector    &GetQueryMode();
    const intVector    &GetNumVars() const;
          intVector    &GetNumVars();

    // Persistence methods
    virtual bool CreateNode(DataNode *node, bool completeSave, bool forceAdd);
    virtual void SetFromNode(DataNode *node);

    // Enum conversion functions
    static std::string QueryType_ToString(QueryType);
    static bool QueryType_FromString(const std::string &, QueryType &);
protected:
    static std::string QueryType_ToString(int);
public:
    static std::string WindowType_ToString(WindowType);
    static bool WindowType_FromString(const std::string &, WindowType &);
protected:
    static std::string WindowType_ToString(int);
public:
    static std::string Groups_ToString(Groups);
    static bool Groups_FromString(const std::string &, Groups &);
protected:
    static std::string Groups_ToString(int);
public:
    static std::string QueryMode_ToString(QueryMode);
    static bool QueryMode_FromString(const std::string &, QueryMode &);
protected:
    static std::string QueryMode_ToString(int);
public:

    // Keyframing methods
    virtual std::string               GetFieldName(int index) const;
    virtual AttributeGroup::FieldType GetFieldType(int index) const;
    virtual std::string               GetFieldTypeName(int index) const;
    virtual bool                      FieldsEqual(int index, const AttributeGroup *rhs) const;

    // User-defined methods
    void AddQuery(const std::string &name, QueryType t, Groups g, WindowType w, int num_input, int allowedVars, QueryMode qMode, int numVars = 1);
    bool QueryExists(const std::string &name, QueryType t);
    int NumberOfInputsForQuery(const std::string &name);
    int AllowedVarsForQuery(const std::string &name);
    bool TimeQueryAvailable(const std::string &name) ;
    int GetWindowType(const std::string &name) ;
    int NumberOfVarsForQuery(const std::string &name);
    bool RegularQueryAvailable(const std::string &name) ;
private:
    stringVector names;
    intVector    types;
    intVector    groups;
    intVector    numInputs;
    intVector    allowedVarTypes;
    intVector    winType;
    intVector    queryMode;
    intVector    numVars;
};

#endif
