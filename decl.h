// scan.c
void reject_token(struct token *t);
int scan(struct token *t);

// tree.c
struct ASTnode *mkastnode(int op, int type,
                          struct ASTnode *left,
                          struct ASTnode *mid,
                          struct ASTnode *right, int intvalue);
struct ASTnode *mkastleaf(int op, int type, int intvalue);
struct ASTnode *mkastunary(int op, int type, struct ASTnode *left, int intvalue);
void dumpAST(struct ASTnode *n, int label, int parentASTop);

// gen.c
int genAST(struct ASTnode *n, int reg, int parentASTop);
void genpreamble();
void genpostamble();
void genfreeregs();
void genprintint(int reg);
void genglobsym(int ind);
int genglobstr(char *strvalue);
int genlabel(void);
int genprimsize(int type);

// cg.c
void freeall_registers();
void cgpreamble();
void cgpostamble();
void cgfuncpreamble(int id);
void cgfuncpostamble(int id);
int cgloadint(int value, int type);
int cgloadglob(int id, int op);
int cgloadlocal(int id, int op);
int cgloadglobstr(int id);
int cgadd(int r1, int r2);
int cgsub(int r1, int r2);
int cgmul(int r1, int r2);
int cgdiv(int r1, int r2);
void cgprintint(int r);
int cgstorglob(int r, int id);
int cgstorlocal(int r, int id);
void cgglobsym(int id);
void cgglobstr(int l, char *strvalue);
int cgcompare_and_set(int ASTop, int r1, int r2);
int cgcompare_and_jump(int ASTop, int r1, int r2, int label);
void cglabel(int l);
void cgjump(int l);
int cgwiden(int r, int oldtype, int newtype);
int cgprimsize(int type);
void cgreturn(int reg, int id);
int cgcall(int id, int numargs);
void cgcopyarg(int r, int argposn);
int cgaddress(int id);
int cgderef(int r, int type);
int cgshlconst(int r, int val);
int cgstorderef(int r1, int r2, int type);
int cgnegate(int r);
int cginvert(int r);
int cglognot(int r);
int cgboolean(int r, int op, int label);
int cgand(int r1, int r2);
int cgor(int r1, int r2);
int cgxor(int r1, int r2);
int cgshl(int r1, int r2);
int cgshr(int r1, int r2);
int newlocaloffset(int type);

// expr.c
struct ASTnode *binexpr(int rbp);

// stmt.c
struct ASTnode *compound_statement(void);

// misc.c
void match(int t, char *what);
void semi(void);
void ident(void);
void lbrace(void);
void rbrace(void);
void lparen(void);
void rparen(void);
void fatal(char *s);
void fatals(char *s1, char *s2);
void fatald(char *s, int d);
void fatalc(char *s, int c);

// sym.c
int findglob(char *s);
int findlocl(char *s);
int findsymbol(char *s);
int addglob(char *name, int type, int stype, int class, int endlabel, int size);
int addlocl(char *name, int type, int stype, int class, int size);
void freeloclsyms(void);
void copyfuncparams(int slot);
void clear_symtable(void);

// decl.c
void var_declaration(int type, int class);
struct ASTnode *function_declaration(int type);
void global_declarations(void);

// types.c
int inttype(int type);
int parse_type(void);
int pointer_to(int type);
int value_at(int type);
struct ASTnode *modify_type(struct ASTnode *tree, int rtype, int op);