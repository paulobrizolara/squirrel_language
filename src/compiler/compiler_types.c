#include "compiler_types.h"
#include "string_helpers.h"
#include "list_helper.h"
#include "symboltable.h"
#include <stdlib.h>


const char * sq_categoryCString(Category category){
    switch(category){
        case categ_unknown:
            return "unknown";
        case categ_primitiveType:
            return "primitiveType";
        case categ_arrayType:
            return "arrayType";
        case categ_structType:
            return "structType";
        case categ_structField:
            return "structField";
        case categ_functionType:
            return "functionType";
        case categ_enumType:
            return "enumType";
        case categ_enumFieldValue:
            return "enumFieldValue";
        case categ_function:
            return "function";
        case categ_variable:
            return "variable";
        case categ_namespace:
            return "namespace";
        default:
            return NULL;
    }
    
}

Expression *sq_Expression( const char *type, const char *exprParam, TypeCategory typeCategory)
{
	Expression *expr = (Expression*)malloc(sizeof(Expression));
	expr->type = cpyString(type);
	expr->expr = cpyString(exprParam);
    expr->typeCategory = typeCategory;
	return expr;
}

Member * sq_Member(const char * name, const char * tableKey, Category category, Member * parent){
    Member * member = (Member *)malloc(sizeof(Member));
    member->name = cpyString(name);
    member->tableKey = cpyString(tableKey);
    member->category = category;
    member->parent = parent;
}
void sq_destroyMember(Member * member){
    if(member != NULL){
        sq_destroyMember(member->parent);
        free(member->name);
        free(member);
    }
}
char * sq_memberToString(const Member * member){
    if(member == NULL){
        return NULL;
    }
    if(member->parent == NULL){
        return cpyString(member->name);
    }
    else{
        char * parentString = sq_memberToString(member->parent);
        char * result = concat3(parentString, ".", member->name);
        free(parentString);
        return result;
    }
}

Expression * sq_memberToExpression(SquirrelContext * sqContext, const Member * member){
    if(member == NULL){
        return sq_Expression("unknown", "", type_invalid);
    }
    
    const char *exprType;
    TypeCategory typeCategory;
    
    if(sq_isCategoryType(member->category)){//member é um tipo
        typeCategory = type_typeliteral;
        TableRow * typeRow = sq_findTypeRow(sqContext, member->tableKey);
        exprType = typeRow == NULL ? "unknown" : typeRow->name;
    }
    else if(member->category == categ_function){//member é uma função
        typeCategory = type_functionliteral;
        exprType = member->tableKey; //nome do tipo é o identificador da função
    }
    else{
        const char * typeName = sq_getMemberType(sqContext, member);
        exprType = typeName == NULL ? "unknown" : typeName;
        
        typeCategory = typeName != NULL 
                                    ? sq_findTypeCategory(sqContext, typeName) 
                                    : type_invalid;
    }
                                            
    char * memberStr = sq_memberToString(member);
    if(typeCategory == type_invalid){
        char * errMsg = concat4("Membro '", memberStr, "' tem tipo inválido ", exprType);
        sq_putError(sqContext, errMsg);
        free(errMsg);
    }
                                                                                                
    return sq_Expression(exprType, memberStr, typeCategory);
}

Parameter * sq_Parameter(const char * typeName, const char * name, arraylist * modifiersList){
	Parameter * param = (Parameter*)malloc(sizeof(Parameter));
	param->type = cpyString(typeName);
	param->name = cpyString(name);
                                                                
	param->isConst = existItem(modifiersList, "const", StrEqualsComparator);
	param->isRef = existItem(modifiersList, "ref", StrEqualsComparator);
	return param;
}

NameDeclItem * sq_NameDeclItem(const char * name, Expression * expr){
	NameDeclItem *nameDecl = (NameDeclItem*)malloc(sizeof(NameDeclItem));
    nameDecl->name = cpyString(name);
    nameDecl->expr = expr;
    return nameDecl;
}

AttributeDecl * sq_AttributeDecl(const char * typeName, NameList * namesList){
	AttributeDecl *atributeDecl = (AttributeDecl*)malloc(sizeof(AttributeDecl));
	atributeDecl->type = cpyString(typeName);
	atributeDecl->namesList = namesList;
	
	return atributeDecl;
}

IfStruct * sq_IfStruct(char * ifId, char * conditional_test, char * block_stmts){
    IfStruct * ifStruct = (IfStruct *)malloc(sizeof(IfStruct));
    ifStruct->ifId = ifId;
    ifStruct->conditional_test = conditional_test;
    ifStruct->block_stmts = block_stmts;
    return ifStruct;
}

NameList * sq_ForHeader(char * initStmt, char * testConditional, char * incrStmt){
    NameList * list = createList(initStmt);
    appendList(list, testConditional);
    appendList(list, incrStmt);
    return list;
}
char * sq_NameDeclToString(void * item){
    NameDeclItem * nameDecl = (NameDeclItem *)item;
    if(nameDecl->expr != NULL){
        return concat3(nameDecl->name, " = ", nameDecl->expr->expr);
    }
    return cpyString(nameDecl->name);
}

char * sq_ParameterToString(void * parameter){
    if(parameter == NULL){
        return cpyString("");
    }
    Parameter * param = (Parameter *)parameter;
    
    char * result = cpyString("");
    if(param->isConst == true){
        appendStr(&result, "const ");
    }
    if(param->isRef == true){
        appendStr(&result, "ref ");
    }
    appendStr(&result, param->type);
    appendStr(&result, " ");
    appendStr(&result, param->name);
    
    return result;
}

char *sq_exprToStr( Expression *expr )
{
    /*switch ( expr->type ) {
        case "int":
            return concat3("cast_intToStr(", expr->expr, ")");
        break;
    }*/
    return cpyString(expr->expr);
}
char * sq_ExpressionStringConverter(void * value){
    return sq_exprToStr((Expression *)value);
}

char * AttributeDeclStringConverter(void * value){
    if(value == NULL){
        return cpyString("");
    }
    AttributeDecl * attributeDecl = (AttributeDecl *)value;
    char * nameDeclListStr = joinList(attributeDecl->namesList, ", ", NULL);
    char * result = concat3(attributeDecl->type, " ", nameDeclListStr);
    free(nameDeclListStr);
    return result;
}

char * attributeListToString(AttributeList * attributeList){
    char * listStr =  joinList(attributeList, ";\n", AttributeDeclStringConverter);
    char * result = concat(listStr, ";");//último ';' não é adicionado por joinList
    free(listStr);
    return result;
}

OperatorCategory sq_getOperatorCategory(const char * op){
    if(strEquals(op, "!") 
        || strEquals(op, "not") 
        || strEquals(op, "||") 
        || strEquals(op, "or") 
        || strEquals(op, "&&") 
        || strEquals(op, "and"))
    {
        return opcategory_logical;
    }
    else 
    if(strEquals(op, "+") 
        || strEquals(op, "-") 
        || strEquals(op, "*") 
        || strEquals(op, "/") 
        || strEquals(op, "%") 
        || strEquals(op, "++") 
        || strEquals(op, "--"))
    {
        return opcategory_arithmetic;
    }
    else 
    if(strEquals(op, "~") 
        || strEquals(op, "|") 
        || strEquals(op, "&") 
        || strEquals(op, ">>") 
        || strEquals(op, "<<"))
    {
        return opcategory_bitwise;
    }
    else 
    if(strEquals(op, "==") 
        || strEquals(op, "!=")
        || strEquals(op, "<")
        || strEquals(op, ">")
        || strEquals(op, "<=")
        || strEquals(op, ">="))
    {
        return opcategory_relational;
    }
    return opcategory_invalid;
}