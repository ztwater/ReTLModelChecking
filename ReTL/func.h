int cnt;
int layer;
int indent;
int pass;
int hier;
FILE * fp;
struct layerEle ** lList;
//struct choices * cList;

extern int yylineno;
FILE * yyin;
int yylex (void);
void yyerror (char const *);

/*
 * NodeType
 * I : int numbers
 * F : float numbers
 * M : negative numbers
 * P : power expressions
 * N : not equal expressions
 * G : greater or equal expressions
 * L : less or equal expressions
 * C : circ, next layer
 * A : add for vectors
 * S : sub for vectors
 * D : dot for vectors
 * E : vector constant (elist)
 * V : vector expressions
 * B : vector expressions astnode
 * K : scalar expressions astnode
 * X : Next
 * U : Until
 * O : For all
 * T : Exists
 * ! : not
 * & : and
 * | : or
 * Y : imply
 * v : vector variables
 * s : scalar variables
 * i : indexed vectors
 * t : true
 * f : false
 * + - * / < > = remains
 */		

/* Different nodes in an AST */
struct ast {
	int nodeType; 
	struct ast *l; 
	struct ast *r;
	int lab;
	int t;
};

struct floatNum {
	int nodeType;
	double val;
};

struct intNum {
	int nodeType;
	int val;
};

struct vecVar {
	char* name;
	int dim;
};

struct scaVar {
	char* name;
};

struct index {
	int nodeType;
	struct ast* a;
	int idx;
};

/* Structures for variables */
#define MAX_SCA_VAR_NUM 1024
#define MAX_VEC_VAR_NUM 32
#define MAX_DIM 32

struct scaVar scaVarTab[MAX_SCA_VAR_NUM];
struct vecVar vecVarTab[MAX_VEC_VAR_NUM];

struct scaVar* sLookUp(char * s);
struct vecVar* vLookUp(char * s, int d);

/* Structures for vector expressions */
struct sList {
	int val;
	struct sList* next;
};

struct eList {
	struct ast* val;
	struct eList* next;
};

struct vecExpr {
	int nodeType;
	struct ast* l; 
	struct ast* r;
	struct sList* m;
};

struct layerEle {
	int lab;
	struct layerEle * next;
};

struct choices{
	int lab;
	struct choices * next;
};

/* Build Variables */
char * createName(char * s, int i);
char * itos(int i);
void bindScalars(struct vecVar* s, int d);
//struct varList* findVars(struct ast * a, struct varList * vl);
//struct varList* findVarsInEList(struct eList * el, struct varList * vl);

/* Build Lists */
struct sList* newSList(int i, struct sList* sl);
struct eList* newEList(struct ast* a, struct eList* el);
struct ast* newVecExpr(int nodeType, struct ast* l, struct ast* r, struct sList* m);

/* Build an AST */
struct ast* newInt(int i);
struct ast* newFloat(double d);
struct ast* newIndex(struct ast* a, int i);
struct ast* newAst(int nodeType, struct ast* l, struct ast* r);

/* Free */
void freeAst(struct ast* a);
void freeEList(struct eList* el);
void freeSList(struct sList* sl);

/* Get original formula */
void getFormula(struct ast* a);
void getEListFormula(struct eList* el);
void getSListFormula(struct sList* sl);

/* Layers */
struct layerEle ** initLayers(int layer);
void printLayers(struct layerEle** lList);
void freeLayers(struct layerEle** lList);
void updateLayer(struct ast* a);
void updateLayerEList(struct eList* el);
int findInLayers(int lab, struct layerEle** lList, int l);

/* Choices for relu */
//struct choices * initChoices();
//void freeChoices(struct choices * cList);
//void addChoices(int lab);

/* Print Loop */
void printIndent();


