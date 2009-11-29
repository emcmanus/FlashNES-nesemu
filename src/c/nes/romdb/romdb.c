#include "nes/romdb/romdb.h"
#include <stdio.h>
#define XML_STATIC
#include <expat.h>

#define BUFFSIZE        8192

static char Buff[BUFFSIZE];
static int Depth;

static romdb_t *romdb;
static romdb_rominfo_t *currom;

static char curel[64];

static romdb_rominfo_t *add(u32 prgcrc,u32 chrcrc,u32 flags)
{
	romdb_rominfo_t *ret = 0;

	ret = (romdb_rominfo_t*)malloc(sizeof(romdb_rominfo_t));
	memset(ret,0,sizeof(romdb_rominfo_t));
	ret->info.prgcrc = prgcrc;
	ret->info.chrcrc = chrcrc;
	ret->info.flags = flags;
	return(ret);
}

static u32 decoderegion(char *s)
{
	u32 ret = 0;

	return(ret);
}

static void XMLCALL
start(void *data, const char *el, const char **attr)
{
  int i;

  for (i = 0; i < Depth; i++)
    printf("  ");

  printf("%s", el);

	if(strcmp(el,"rom") == 0) {
		u32 prgcrc = 0,chrcrc = 0,flags = 0;
		char *end;

		for (i = 0; attr[i]; i += 2) {
			if(strcmp(attr[i],"prgcrc") == 0)
				prgcrc = strtol(attr[i+1],&end,16);
			if(strcmp(attr[i],"chrcrc") == 0)
				chrcrc = strtol(attr[i+1],&end,16);
			if(strcmp(attr[i],"region") == 0)
				flags |= decoderegion(attr[i+1]);
//			printf(" %s='%s'", attr[i], attr[i + 1]);
		}
		currom = add(prgcrc,chrcrc,flags);
  }

	strcpy(curel,el);
  printf("\n");
  Depth++;
}

static void XMLCALL
end(void *data, const char *el)
{
	if(strcmp(el,"rom") == 0)
		currom = 0;
  Depth--;
  strcpy(curel,"");
}

static char *eatwhite(char *s)
{
	while((*s == ' ' || *s == '\t' || *s == '\n') && *s)
		s++;
	return(s);
}

static void XMLCALL
chardata(void *userData,const XML_Char *s,int len)
{
	char str[1024],*p;
	int i;

	for(i=0;i<len;i++)
		str[i] = s[i];
	str[i] = 0;
	p = eatwhite(str);
	if(currom) {
		if(*p == 0)
			return;
		for (i = 0; i < Depth; i++)
			printf("  ");
		if(strcmp(curel,"name") == 0) {
			currom->info.name = (char*)malloc(len);
			strcpy(currom->info.name,p);
		}
		if(strcmp(curel,"board") == 0) {
			currom->mapper.unif.board = (char*)malloc(len);
			strcpy(currom->mapper.unif.board,p);
		}
		printf("'%s'\n",p);
	}
}

romdb_t *romdb_load(char *fn)
{
	romdb_t *ret = 0;
	FILE *fp;
	XML_Parser p = XML_ParserCreate(NULL);

	romdb = ret = (romdb_t*)malloc(sizeof(romdb_t));

	if (! p) {
		fprintf(stderr, "Couldn't allocate memory for parser\n");
		exit(-1);
	}

	XML_SetElementHandler(p, start, end);
	XML_SetCharacterDataHandler(p,chardata);
	if((fp = fopen(fn,"rt")) == 0) {
		log_error("cannot open rom database '%s'\n",fn);
		return(0);
	}
	for (;;) {
		int done;
		int len;

		len = (int)fread(Buff, 1, BUFFSIZE, fp);
		printf("read = '%s'\n",Buff);
		if (ferror(fp)) {
			fprintf(stderr, "Read error\n");
			return(0);
		}
		done = feof(fp);

		if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
			fprintf(stderr, "Parse error at line %u:\n%s\n",
				XML_GetCurrentLineNumber(p),
				XML_ErrorString(XML_GetErrorCode(p)));
			return(0);
		}
		if(done)
			break;
	}
	fclose(fp);
	XML_ParserFree(p);
	return(ret);
}

//extras for inserting items/rewriting/outputting romdb (not needed)
int romdb_save(char *fn,romdb_t *db)
{
	return(1);
}

int romdb_add()
{
	return(1);
}
