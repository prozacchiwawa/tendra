/*
 * Copyright 2011, The TenDRA Project.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 * Automatically generated by lexi version 2.0
 */

#include "lexi_lexer.h"

	#include <shared/bool.h>
	#include <shared/check.h>
	#include <shared/error.h>

	#include "lexer.h"

#include <assert.h>
int lexi_readchar(struct lexi_state *state) {
	if (state->buffer_index) {
		return lexi_pop(state);
	}

	return lexi_getchar();
}
void lexi_push(struct lexi_state *state, const int c) {
	assert(state);
	assert(state->buffer_index < sizeof state->buffer / sizeof *state->buffer);
	state->buffer[state->buffer_index++] = c;
}

int lexi_pop(struct lexi_state *state) {
	assert(state);
	assert(state->buffer_index > 0);
	return state->buffer[--state->buffer_index];
}

void lexi_flush(struct lexi_state *state) {
	state->buffer_index = 0;
}


/* LOOKUP TABLE */

typedef unsigned char lookup_type;
static lookup_type lookup_tab[] = {
	   0,    0,    0,    0,    0,    0,    0,    0,    0,  0x1,  0x1,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,  0x1,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,  0x4,    0,    0, 
	 0x4,  0x4,  0x4,  0x4,  0x4,  0x4,  0x4,  0x4,  0x4,  0x4,    0,    0, 
	   0,    0,    0,    0,    0,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe, 
	 0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe, 
	 0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,    0,    0,    0,    0,  0xc, 
	   0,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe, 
	 0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe,  0xe, 
	 0xe,  0xe,  0xe,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0
};

int lexi_group(enum lexi_groups group, int c) {
	if (c == LEXI_EOF) {
		return 0;
	}
	return lookup_tab[c] & group;
}


/* PRE-PASS ANALYSERS */

void lexi_init(struct lexi_state *state) {
	state->zone = lexi_read_token;
	state->buffer_index = 0;
}
/* ZONES PASS ANALYSER PROTOTYPES*/

static void lexi_read_token_bracketed_comment(struct lexi_state *state);
static void lexi_read_token_singleline_comment(struct lexi_state *state);
/* MAIN PASS ANALYSERS */


/* MAIN PASS ANALYSER for bracketed_comment */
static void
lexi_read_token_bracketed_comment(struct lexi_state *state)
{
	start: {
		int c0 = lexi_readchar(state);
		switch (c0) {
		case LEXI_EOF: {
				/* ACTION <eof_in_comment> */
				{

	error_posn(ERR_FATAL, istream_name(&lexer_stream->istream), (int) istream_line(&lexer_stream->istream),
		"end of file in comment");
				}
				/* END ACTION <eof_in_comment> */
				goto start;	/* leaf */
			}

		case '*': {
				int c1 = lexi_readchar(state);
				if (c1 == '/') {
					return;
				}
				lexi_push(state, c1);
			}
			break;

		}

		/* DEFAULT */
		goto start; /* DEFAULT */
	}
}

/* MAIN PASS ANALYSER for singleline_comment */
static void
lexi_read_token_singleline_comment(struct lexi_state *state)
{
	start: {
		int c0 = lexi_readchar(state);
		switch (c0) {
		case LEXI_EOF: {
				/* ACTION <eof_in_comment> */
				{

	error_posn(ERR_FATAL, istream_name(&lexer_stream->istream), (int) istream_line(&lexer_stream->istream),
		"end of file in comment");
				}
				/* END ACTION <eof_in_comment> */
				goto start;	/* leaf */
			}

		case '\n': {
				return;
			}

		}

		/* DEFAULT */
		goto start; /* DEFAULT */
	}
}

/* MAIN PASS ANALYSER for the global zone */
int
lexi_read_token(struct lexi_state *state)
{
	if (state->zone != lexi_read_token)
		return state->zone(state);
	start: {
		int c0 = lexi_readchar(state);
		if (lexi_group(lexi_group_white, c0)) goto start;
		switch (c0) {
		case '"': {
				/* ACTION <read_basic> */
				{
					int ZT1;

       ZT1=read_basic();
					return ZT1;
				}
				/* END ACTION <read_basic> */
				goto start;	/* leaf */
			}

		case '%': {
				/* ACTION <read_builtin> */
				{
					int ZT1;

       ZT1=read_builtin();
					return ZT1;
				}
				/* END ACTION <read_builtin> */
				goto start;	/* leaf */
			}

		case '/': {
				int c1 = lexi_readchar(state);
				switch (c1) {
				case '*': {
						lexi_read_token_bracketed_comment(state);
						goto start;	/* pure function */
					}

				case '/': {
						lexi_read_token_singleline_comment(state);
						goto start;	/* pure function */
					}

				}
				lexi_push(state, c1);
			}
			break;

		case '-': {
				int c1 = lexi_readchar(state);
				if (c1 == '>') {
					return LEXER_TOK_ARROW;
				}
				lexi_push(state, c1);
			}
			break;

		case ')': {
				return LEXER_TOK_CLOSE_HTUPLE;
			}

		case '(': {
				return LEXER_TOK_OPEN_HTUPLE;
			}

		case ']': {
				return LEXER_TOK_END_HSCOPE;
			}

		case '[': {
				return LEXER_TOK_BEGIN_HSCOPE;
			}

		case '}': {
				return LEXER_TOK_END_HRULE;
			}

		case '{': {
				return LEXER_TOK_BEGIN_HRULE;
			}

		case '>': {
				return LEXER_TOK_END_HACTION;
			}

		case '<': {
				return LEXER_TOK_BEGIN_HACTION;
			}

		case ';': {
				return LEXER_TOK_TERMINATOR;
			}

		case '&': {
				return LEXER_TOK_REFERENCE;
			}

		case '!': {
				return LEXER_TOK_IGNORE;
			}

		case '?': {
				return LEXER_TOK_PRED_HRESULT;
			}

		case '=': {
				return LEXER_TOK_DEFINE;
			}

		case ',': {
				return LEXER_TOK_SEPARATOR;
			}

		case '$': {
				return LEXER_TOK_EMPTY;
			}

		case '#': {
				int c1 = lexi_readchar(state);
				if (c1 == '#') {
					return LEXER_TOK_HANDLER_HSEP;
				}
				lexi_push(state, c1);
			}
			break;

		case '|': {
				int c1 = lexi_readchar(state);
				if (c1 == '|') {
					return LEXER_TOK_ALT_HSEP;
				}
				lexi_push(state, c1);
			}
			break;

		case ':': {
				int c1 = lexi_readchar(state);
				if (c1 == ':') {
					return LEXER_TOK_SCOPEMARK;
				}
				lexi_push(state, c1);
				return LEXER_TOK_TYPEMARK;
			}

		}
		if (lexi_group(lexi_group_identstart, c0)) {
			/* ACTION <read_identifier> */
			{
				int ZT1;

	ZT1=read_identifier(c0);
				return ZT1;
			}
			/* END ACTION <read_identifier> */
			goto start;	/* leaf */
		}

		/* DEFAULT */
		/* ACTION <lexi_unknown_token> */
		{
			int ZT1;

	ZT1= lexi_unknown_token(c0);
			return ZT1;
		}
		/* END ACTION <lexi_unknown_token> */
		goto start; /* DEFAULT */
	}
}


