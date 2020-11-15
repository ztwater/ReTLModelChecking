/* 
 * Functions for parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "func.h"

/* variable table */
/* hash a symbol */
static unsigned hash(char * s) {
	unsigned int h = 0;
	char * tmp = s;
	unsigned c = *tmp;
	while(c) {
		h = (h*9)^c;
		c = *(++tmp);
	}
	return h;
}

struct scaVar* sLookUp(char * s) {
	struct scaVar* p = &scaVarTab[hash(s)%MAX_SCA_VAR_NUM]; 
	int scnt = MAX_SCA_VAR_NUM;
	while (scnt > 0) {
		/* new entry */
		if (!p->name) {
			p->name = strdup(s);
			fprintf(fp, "%s = s.symbols('%s')\n", p->name, p->name);
			return p;
		}
		/* find successfully */
		if (p->name && !strcmp(p->name, s)) return p;
		/* search the table linearly */
		p++; 
		/* reach the end of the table */
		if (p >= scaVarTab + MAX_SCA_VAR_NUM) p = scaVarTab;
		scnt--;
	}
	yyerror("Scalar variable table overflow!\n");
	abort();
}

struct vecVar* vLookUp(char * s, int d) {
	struct vecVar* p = &vecVarTab[hash(s)%MAX_VEC_VAR_NUM]; 
	int vcnt = MAX_VEC_VAR_NUM;

	while (vcnt > 0) {
		/* new entry */
		if (!p->name) {
			p->name = strdup(s);
			p->dim = d;
			return p;
		}

		/* find successfully */
		if (p->name && !strcmp(p->name, s)) return p;
		/* search the table linearly */
		p++; 
		/* reach the end of the table */
		if (p >= vecVarTab + MAX_VEC_VAR_NUM) p = vecVarTab;
		vcnt--;
	}
	yyerror("Vector variable table overflow\n");
	abort();
}

char * itos(int i) {
	char * tmp = malloc(sizeof(char)*12);
	char * idx = malloc(sizeof(char)*12);
	int j = 0;
	do {
		tmp[j++] = '0' + i % 10;
	} while ((i/=10) > 0);
	for (int k = 0; k < j; k++) {
		idx[k] = tmp[j-1-k];
	}
	idx[j] = '\0';
	free(tmp);
	return idx;
}

char * createName(char * s, int i) {
	char * tmp = strdup(s);
	char * idx = itos(i);
	strcat(tmp, "_");
	strcat(tmp, idx);
	free(idx);
	return tmp;
}

void bindScalars(struct vecVar* s, int d) {
	/* add the dim of the vector */
	if (d > MAX_DIM) yyerror("Too many dims");
	s = vLookUp(s->name, d);
	char ** l = malloc(sizeof(char*) *d); 
	for (int i = 1; i <= d; i++) {
		l[i-1]= createName(s->name, i);
		sLookUp(l[i-1]);
	}
	if (pass == 0) {
		fprintf(fp, "%s = [", s->name);
		for (int i = 0; i < d; i++) {
			fprintf(fp, "%s", l[i]);
			if (i != d-1) fprintf(fp, ", ");
			else fprintf(fp, "]\n");	
		}
		fprintf(fp, "var_list += %s\n", s->name);
	}
	free(l);
	return;
}

/* make ast nodes */
struct ast* newInt(int i) {
	struct intNum* a = malloc(sizeof(struct intNum));
	if (!a) {
		yyerror("Out of space");
		exit(0);
	}
	a->nodeType = 'I';
	a->val = i;
	return (struct ast*) a;
}

struct ast* newFloat(double d) {
	struct floatNum* a = malloc(sizeof(struct floatNum));
	if (!a) {
		yyerror("Out of space");
		exit(0);
	}
	a->nodeType = 'F';
	a->val = d;
	return (struct ast*) a;
}

struct ast* newIndex(struct ast * a, int i) {
	struct index* v = malloc(sizeof(struct index));
	if (!v) {
		yyerror("Out of space");
		exit(0);
	}
	v->nodeType = 'i';
	v->a = a;
	v->idx = i;
	return (struct ast*) v;
}

struct sList* newSList(int i, struct sList* sl) {
	struct sList* l = malloc(sizeof(struct sList));
	if (!l) {
		yyerror("Out of space");
		exit(0);
	}
	l->val = i;
	l->next = sl;
	return l;
}

struct eList* newEList(struct ast* a, struct eList* el) {
	struct eList* l = malloc(sizeof(struct eList));
	if (!l) {
		yyerror("Out of space");
		exit(0);
	}
	l->val = a;
	l->next = el;
	return l;
}

struct ast* newAst(int nodeType, struct ast* l, struct ast* r) {
	struct ast* a = malloc(sizeof(struct ast));
	if (!a) {
		yyerror("Out of space");
		exit(0);
	}
	a->nodeType = nodeType;
	a->l = l;
	a->r = r;
	return a;
}

struct ast* newVecExpr(int nodeType, struct ast* l, struct ast* r, struct sList* m) {
	struct vecExpr* a = malloc(sizeof(struct vecExpr));
	if (!a) {
		yyerror("Out of space");
		exit(0);
	}
	a->nodeType = nodeType;
	a->l = l;
	a->r = r;
	a->m = m;
	return (struct ast*)a;
}

void freeAst(struct ast* a) {
	if (!a) {
		yyerror("Null ast!");
		return;
	}
	switch (a->nodeType) {
		/* More than two subtrees */
	case 'V':
		freeSList(((struct vecExpr*)a)->m);

		/* Two subtrees */
	case 'U':
	case 'O':
	case 'T':

	case 'Y':
	case '|':
	case '&':

	case 'A':
	case 'S':

	case '=':
	case '>':
	case '<':
	case 'N':
	case 'G':
	case 'L':

	case '+':
	case '-':
	case '*':
	case '/':
	case 'P':

	case 'D':
		freeAst(a->r);

		/* One subtree */
	case 'X':
	case '!':
	case 'C':
	case 'M':
	case 'B':
	case 'K':
	case 't':
	case 'f':
		freeAst(a->l);

		/* No subtrees */
	case 'I':
	case 'F':
	case 'v':
	case 's':
		break;

		/* Others */
	case 'E':
		freeEList(((struct eList*)(a->l)));
		break;

	case 'i':
		freeAst(((struct index*)a)->a);
		break;
	}
	/* print expressions in each layer */
	if (pass == 0) {
		if (a->nodeType == 'X' || a->nodeType == 'U' || a->nodeType == 'O' || a->nodeType == 'T' ||
		a->nodeType == '|' || a->nodeType == '&' || a->nodeType == 't' || a->nodeType == 'f' ||
		a->nodeType == '!' || a->nodeType == 'A' || a->nodeType == 'S' || a->nodeType == 'C' ||
		a->nodeType == 'E' || a->nodeType == 'Y' || a->nodeType == 'K' || a->nodeType == 'B') {
			if (a->t >= layer) {
				yyerror("Layer out of index");
				fprintf(fp, "e%d: %d", a->lab, a->t);
				exit(0);
			}
			struct layerEle * le = malloc(sizeof(struct layerEle));
			if (!le) {
				yyerror("Out of space");
				exit(0);
			}
			le->lab = a->lab;
			le->next = NULL;
			if (lList[a->t] == NULL) {
				lList[a->t] = le;
			}
			else {
				le->next = lList[a->t];
				lList[a->t] = le;
			}
		}
	}
	free(a);
}

void freeEList(struct eList* el) {
	if (el == NULL) return;
	freeAst(el->val);
	freeEList(el->next);
	free(el);
}

void freeSList(struct sList* sl) {
	if (sl == NULL) return;
	freeSList(sl->next);
	free(sl);
}

void printIndent() {
	for (int j = 0; j < indent; j++) {
		fprintf(fp, "\t");
	}
	return;
}

/* print the formula */
void getFormula(struct ast* a) {
	if (!a) {
		yyerror("Null ast!");
		return;
	}

	if (pass == 0) {
		// update layer lab recursively
		if (a->nodeType == 'X') updateLayer(a->l); 
		// count forall vars
		if (a->nodeType == 'O') {
			if (hier == 0) hier = 1;
			if (a->l->nodeType == 's') {
				fprintf(fp, "all_list.append(%s)\n", ((struct scaVar*)a->l->l)->name);
			}
			else {
				fprintf(fp, "all_list += %s\n", ((struct vecVar*)a->l->l)->name);
			}
		}
		if (a->nodeType == 'T') {
			if (hier == 0) hier = -1;
			if (a->l->nodeType == 's') {
				fprintf(fp, "xst_list.append(%s)\n", ((struct scaVar*)a->l->l)->name);
			}
			else {
				fprintf(fp, "xst_list += %s\n", ((struct vecVar*)a->l->l)->name);
			}
		}
		return;
	}

	switch (a->nodeType) {
	/* int or float */
	case 'I': fprintf(fp, "%d", ((struct intNum*)a)->val); break;
	case 'F': fprintf(fp, "%lf", ((struct floatNum*)a)->val); break;
	/* variables */
	case 's': fprintf(fp, "%s", ((struct scaVar*)a->l)->name); break;
	case 'v': fprintf(fp, "%s",((struct vecVar*)a->l)->name); break;
	/* scalar operations */
	case 'M': fprintf(fp, "-"); getFormula(a->l); break;
	case 'P': getFormula(a->l); fprintf(fp, "**"); getFormula(a->r); break;
	case '*': getFormula(a->l); fprintf(fp, "*"); getFormula(a->r); break;
	case '/': getFormula(a->l); fprintf(fp, "/"); getFormula(a->r); break;
	case '+': getFormula(a->l); fprintf(fp, "+"); getFormula(a->r); break;
	case '-': getFormula(a->l); fprintf(fp, "-"); getFormula(a->r); break;

	case '=': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", '=', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	case '>': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", '>', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	case '<': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", '<', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	case 'N': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", 'N', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	case 'G': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", 'G', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	case 'L': fprintf(fp, "[np.array([["); getFormula(a->l); fprintf(fp, ", 'L', "); getFormula(a->r); fprintf(fp, "]], dtype=object)]"); break;
	/* index */
	case 'i': 
		// if the vector is a variable, then get its name
		if (((struct index*)a)->a->nodeType == 'v') {
			getFormula(a->l);
		}
		// if the vector is a CIRC, then get its i-th choice
		else if(((struct index*)a)->a->nodeType == 'C' && findInLayers(((struct index*)a)->a->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->l->lab, a->l->lab);
		}
		else fprintf(fp, "e%d", a->l->lab);
		fprintf(fp, "[%d]", ((struct index*)a)->idx); 
		break;
	/* dot product */
	case 'D': 
		fprintf(fp, "s.vec_dot(");
		if (a->l->nodeType == 'v') {
			getFormula(a->l);
		}
		else if(a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->l->lab, a->l->lab);
		}
		else fprintf(fp, "e%d", a->l->lab);
		fprintf(fp, ", ");
		if (a->r->nodeType == 'v') {
			getFormula(a->r);
		}
		else if(a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->r->lab, a->r->lab);
		}
		else fprintf(fp, "e%d", a->r->lab);
		fprintf(fp, ")");
		break;
	/* vector */
	case 'E': printIndent(); fprintf(fp, "e%d = ", cnt); fprintf(fp, "["); getEListFormula(((struct eList*)(a->l))); fprintf(fp, "\n"); break;
	case 'A': 
		printIndent();
		fprintf(fp, "e%d = s.vec_add(", cnt);
		if (a->l->nodeType == 'v') {
			getFormula(a->l);
		}
		else if(a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->l->lab, a->l->lab);
		}
		else fprintf(fp, "e%d", a->l->lab);
		fprintf(fp, ", ");
		if (a->r->nodeType == 'v') {
			getFormula(a->r);
		}
		else if(a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->r->lab, a->r->lab);
		}
		else fprintf(fp, "e%d", a->r->lab);
		fprintf(fp, ")\n"); 
		break;
	case 'S': 
		printIndent();
		fprintf(fp, "e%d = s.vec_sub(", cnt);
		if (a->l->nodeType == 'v') {
			getFormula(a->l);
		}
		else if(a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->l->lab, a->l->lab);
		}
		else fprintf(fp, "e%d", a->l->lab);
		fprintf(fp, ", ");
		if (a->r->nodeType == 'v') {
			getFormula(a->r);
		}
		else if(a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d]", a->r->lab, a->r->lab);
		}
		else fprintf(fp, "e%d", a->r->lab);
		fprintf(fp, ")\n"); 
		break;
	case 'C': 
		printIndent();
		fprintf(fp, "e%d = s.circ(", cnt);
		if (a->l->nodeType == 'v') {
			getFormula(a->l);
			fprintf(fp, ", 0");
		}
		else if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) {
			fprintf(fp, "e%d[0][a_%d], %d", a->l->lab, a->l->lab, a->l->lab);
		}
		else fprintf(fp, "e%d, %d", a->l->lab, a->l->lab);
		fprintf(fp, ", layers, M, b)\n");
		// print a loop for relu-elimination
		if (findInLayers(a->lab, lList, layer-1) == 0) {
			printIndent(); 
			fprintf(fp, "for a_%d in range(s.get_dim(%d, d, layers)):\n", cnt, cnt);
			indent++;
			printIndent();
			if (indent == 1) fprintf(fp, "extra_inequalities = [e%d[1][a_%d]]\n", a->lab, a->lab);
			else fprintf(fp, "extra_inequalities += [e%d[1][a_%d]]\n", a->lab, a->lab);
			//fprintf(fp, "extra_inequalities += [e%d[1][a_%d]]\n", a->lab, a->lab);
		}
		break;
	case 'V':
		fprintf(fp, "s.get_inequalities(");
		if (a->l->nodeType == 'v') getFormula(a->l);
		else if(a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d]", a->l->lab, a->l->lab);
		else fprintf(fp, "e%d", a->l->lab);
		fprintf(fp, ", e%d, ", ((struct vecExpr*)a)->m->val);
		if (a->r->nodeType == 'v') getFormula(a->r);
		else if(a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d]", a->r->lab, a->r->lab);
		else fprintf(fp, "e%d", a->r->lab);
		fprintf(fp, ")");
		break;	
	/* logic units */
	case 't': 
	case 'f':
	case 'K':
	case 'B':
		printIndent(); 
		fprintf(fp, "e%d = ", cnt); 
		getFormula(a->l); 
		fprintf(fp, "\n"); 
		break;
	/* logic operations */
	case '!': 
		printIndent(); 
		fprintf(fp, "e%d = s.NOT", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d])\n", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d)\n", a->l->lab); 
		break;
	case '&': 
		printIndent(); 
		fprintf(fp, "e%d = s.AND", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d], ", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d, ", a->l->lab); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d])\n", a->r->lab, a->r->lab);
		else fprintf(fp, "e%d)\n", a->r->lab); 
		break;
	case '|': 
		printIndent(); 
		fprintf(fp, "e%d = s.OR", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d], ", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d, ", a->l->lab); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d])\n", a->r->lab, a->r->lab);
		else fprintf(fp, "e%d)\n", a->r->lab); 
		break;
	case 'Y': 
		printIndent(); 
		fprintf(fp, "e%d = s.OR(s.NOT", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d]), ", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d), ", a->l->lab); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d])\n", a->r->lab, a->r->lab);
		else fprintf(fp, "e%d)\n", a->r->lab); 
		break;
	/* temporal operations */
	case 'X': 
		printIndent(); 
		fprintf(fp, "e%d = s.Next", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d])\n", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d)\n", a->l->lab); 
		break;
	case 'U': 
		printIndent();
		fprintf(fp, "e%d = s.Until", cnt); 
		if (a->l->nodeType == 'C' && findInLayers(a->l->lab, lList, layer-1) == 0) fprintf(fp, "(e%d[0][a_%d], ", a->l->lab, a->l->lab);
		else fprintf(fp, "(e%d, ", a->l->lab); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, "e%d[0][a_%d], %d, layers)\n", a->r->lab, a->r->lab, cnt);
		else fprintf(fp, "e%d)\n", a->r->lab); 
		break;
	/* forall and exists */
	case 'O': 
		printIndent(); 
		fprintf(fp, "e%d = s.Forall(", cnt); 
		getFormula(a->l); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, ", e%d[0][a_%d])\n", a->r->lab, a->r->lab);
		else fprintf(fp, ", e%d)\n", a->r->lab); 
		break;
	case 'T': 
		printIndent(); 
		fprintf(fp, "e%d = s.Exists(", cnt); 
		getFormula(a->l); 
		if (a->r->nodeType == 'C' && findInLayers(a->r->lab, lList, layer-1) == 0) fprintf(fp, ", e%d[0][a_%d])\n", a->r->lab, a->r->lab);
		else fprintf(fp, ", e%d)\n", a->r->lab); 
		break;
	}
	return;
}


void getEListFormula(struct eList* el) {
	if (el == NULL) {
		fprintf(fp, "]"); 
		return;
	}
	getFormula(el->val);
	if (el->next != NULL) fprintf(fp, ", ");
	getEListFormula(el->next);
}

void getSListFormula(struct sList* sl) {
	if (sl == NULL) {
		fprintf(fp, "]\n"); 
		return;
	}
	fprintf(fp, "'%c'", sl->val);
	if (sl->next != NULL) fprintf(fp, ", ");
	getSListFormula(sl->next);
}

/* Layer operations */
struct layerEle ** initLayers(int layer) {
	struct layerEle ** l = malloc(sizeof(struct layerEle *)*layer);
	if (!l) {
		yyerror("Out of space");
		exit(0);
	}
	for (int i = 0; i < layer; i++) {
		l[i] = NULL;
	}
	return l;
}

void printLayers(struct layerEle** lList) {
	for (int i = 0; i < layer; i++) {
		fprintf(fp, "layer_%d = [", i);
		struct layerEle* tmp = lList[i];
		while (tmp != NULL) {
			fprintf(fp, "%d", tmp->lab);
			if (tmp->next != NULL) fprintf(fp, ", ");
			tmp = tmp->next;
		}
		fprintf(fp, "]\n");
	}
	fprintf(fp, "layers = [");
	for (int i = 0; i < layer; i++) {
		fprintf(fp, "layer_%d", i);
		if (i != layer-1) fprintf(fp, ", ");
		else fprintf(fp, "]\n");
	}
}

void freeLayers(struct layerEle** lList) {
	for (int i = 0; i < layer; i++) {
		while (lList[i] != NULL) {
			struct layerEle* tmp = lList[i];
			lList[i] = tmp->next;
			free(tmp);
		}
	}
	free(lList);
}

int findInLayers(int lab, struct layerEle** lList, int l) {
	struct layerEle * tmp = lList[layer-1];
	while(tmp != NULL) {
		if (tmp->lab == lab) return 1;
		tmp = tmp->next;
	}
	return 0;
}

void updateLayer(struct ast* a) {
	if (!a) {
		yyerror("Null ast!");
		return;
	}

	switch (a->nodeType) {
		/* More than two subtrees */
	case 'V':
		/* Two subtrees */
	case 'U':
	case 'O':
	case 'T':

	case 'Y':
	case '|':
	case '&':

	case 'A':
	case 'S':

	case '=':
	case '>':
	case '<':
	case 'N':
	case 'G':
	case 'L':

	case '+':
	case '-':
	case '*':
	case '/':
	case 'P':

	case 'D':
		updateLayer(a->r);

		/* One subtree */
	case 'X':
	case '!':
	case 'C':
	case 'M':
	case 'B':
	case 'K':
	case 't':
	case 'f':
		updateLayer(a->l);

		/* No subtrees */
	case 'I':
	case 'F':
	case 'v':
	case 's':
		break;

		/* Others */
	case 'i':
		updateLayer(((struct index*)a)->a);
		break;
	}
	if (a->nodeType == 'X' || a->nodeType == 'U' || a->nodeType == 'O' || a->nodeType == 'T' ||
		a->nodeType == '|' || a->nodeType == '&' || a->nodeType == 't' || a->nodeType == 'f' ||
		a->nodeType == '!' || a->nodeType == 'A' || a->nodeType == 'S' || a->nodeType == 'C' ||
		a->nodeType == 'E' || a->nodeType == 'Y' || a->nodeType == 'K' || a->nodeType == 'B') {
		(a->t)++;
	}
	return;
}