Indentation
-----------

Use real tabs. Visually 1 tab = 4 spaces.


Brackets
--------

Place a starting bracket at the end of the line,
and a closing bracket on the separate line (except `else`).

		if ( ... ) {
			...
		} else {
			...
		}


Spaces
------

Place a space before any logical symbol on a line (except a starting bracket).

		int x = 16;

		x = call( 3 );

Pad any content inside curly, squared or round brackets with spaces.

		char	str[ 16 ];

		while ( x > ( func( a, b ) ) ) {
			...
		}


Alignment
---------

Align variable and function declarations by sutable number of tabs.

		int		x = 2;
		char*	str;
		size_t	func( int A );

If comments are at the end of a line. Align them with tabs appropriately.

		if ( a < 3 ) {
			return 1;		// the first comment
		} else {
			return 0;		// the second comment
		}

Vertical separation
-------------------

Write code as a book. Devide the contents by logical blocks.

		prepareX();
		prepareY();

		x = usingX();

		destroyX();
		destroyY();

In particular blocks declaring variables or functions have to be standalone.

		void function() {
			int var1;
			char var2;

			if ( var1 ) {
				return var2;
			} else {
				return '?';
			}
		}

Naming
------

Function names start with capital letter.
Words are sticked together. Each word starts is capitalized.
The first word should be a verb.
No articles (`a`, `the`).

		void CreateStructure();
		void FindSpacesInString();

Variable names start with a lower character.
Words are sticked and capitalized.

		int		someVar;
		char*	anotherVar;

Local (not exported) functions and variables should be declared as `static`.
Names of them start with `_` symbol.

		static char		_globalVariable[ 3 ];

		static int		_CountPeople();
		static char		_GetSymbol();

User defined types are always ending with `_t`

		typedef	checksum_t;

		typedef struct man_s {
			int name;
		} man_t;

Macro definitions are written with all capitals.
Words are separated by `_` symbol.

		#define SOME_CONST_VAR	10
		#define MACRO_1()		( 3 + 5 )


Const
-----

Try to write in a functional style (where possible).
Place `const` each time if this variable or an argument will not be modified.

		const int y = 65535;

		// a constant pointer to a constant integer
		void x( const int* const ptr );

Do not use: `int const * ptr;`


Library
-------

All public (exported) symbols and definitions must be prepended by a special prefix.
In case of **BigBenBox** it is `bbb`, `Bbb` and `BBB_`.

		// use "BBB_" for preprocessor instructions
		#define BBB_STATUS_BAR		"string"
		#define BBB_MACRO_ONE()		( 3 + 5 )

		// use "bbb" for variables
		int		bbbOneHundredFiftySix = 156;
		char	bbbExported = 0;

		// use "Bbb" for functions
		void	BbbGetValue( int x );
		void	BbbSomeFunction();


Files
-----

> *TODO*
