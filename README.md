## Idea

This was an attempt to make a privately hosted file storage, similar to Dropbox.  
The idea was to build a native application, able to handle filesystem events and synchronize files via a server.  
It's not finished, and never will be. I'm hosting it here for demonstration purposes only.

## Implementation

During a synchronization process only a difference between file versions is transfered over the network.  
Files are hashed into parts, which are stored on the server in trees, just like in a Git repository.  
Thus, it's possible to see the whole history of a node, and rollback any changes.

The code is cross-platform (compiles on Linux, OSX, and Windows). It's written in C, and source files are preprocessed with [PerlPP](https://github.com/d-ash/perlpp). This preprocessor turned out to be very helpful as it's much more flexible than the standard C-preprocessor.

For example, it's very important to correctly manage memory in C programs.  
In the code below, pay attention to `<<<<<<<<<<<<<<<<<<<<<<<`. Cleanup actions `$onCleanup` are placed near the code, where this memory is allocated. However, all of the cleanup will be done at the very end `$cleanup`. The cleanup will be executed even if any internal `$call` fails.

```C
static bbb_result_t
_ProcessEntry( const char* const path, const size_t skip, const char* const name, @_t* const ss ) {

  /* ... */
  
	$call bbb_util_Malloc( ( void** )&entry, sizeof( @_entry_t ) + pathMem );
	$onCleanup              // <<<<<<<<<<<<<<<<<<<<<<<
		if ( BBB_FAILED( result ) ) {
			free( entry );
		}
	$$                      // <<<<<<<<<<<<<<<<<<<<<<<

	$call bbb_util_Malloc( ( void** )&fullPath, pathMem + skip + 1 );
	$onCleanup              // <<<<<<<<<<<<<<<<<<<<<<<
		free( fullPath );
	$$                      // <<<<<<<<<<<<<<<<<<<<<<<

	if ( stat( fullPath, &entryInfo ) ) {
		BBB_ERR( BBB_ERROR_FILESYSTEMIO, "Cannot get info about %s: %s", fullPath, strerror( errno ) );
		result = BBB_ERROR_FILESYSTEMIO;
		$gotoCleanup;         // <<<<<<<<<<<<<<<<<<<<<<<
	}

  /* ... */
  
	if ( S_ISDIR( entryInfo.st_mode ) ) {
		entry->status |= @^ENTRY_STATUS_DIR;
		$call _AddToSnapshot( entry, ss );
		$call _ProcessDir( fullPath, skip, ss );
	} else if ( S_ISREG( entryInfo.st_mode ) ) {
		entry->status &= ~@^ENTRY_STATUS_DIR;
		$call _AddToSnapshot( entry, ss );
	} else {
		BBB_LOG( "Skipping irregular file: %s", fullPath );
		$gotoCleanup;         // <<<<<<<<<<<<<<<<<<<<<<<
	}

	$cleanup;               // <<<<<<<<<<<<<<<<<<<<<<<
	return result;
}
```

The resulting C-code looks like:
```C
static bbb_result_t
_ProcessEntry( const char* const path, const size_t skip, const char* const name, bbb_sshot_t* const ss ) {
       /* ... */
       
       pathMem = ( strlen( path ) - skip + strlen( name ) + 1 + BBB_WORD_SIZE ) & ~( BBB_WORD_SIZE - 1 );

       if ( BBB_FAILED( result =  bbb_util_Malloc( ( void** )&entry, sizeof( bbb_sshot_entry_t ) + pathMem )  ) ) { 
           goto L_cleanup_0;
       }   

       if ( BBB_FAILED( result =  bbb_util_Malloc( ( void** )&fullPath, pathMem + skip + 1 )  ) ) { 
           goto L_cleanup_1;
       }
       
       /* ... */
       
L_cleanup_2:
       free( fullPath );

L_cleanup_1:
       if ( BBB_FAILED( result ) ) {
           free( entry );
       }

L_cleanup_0:
       return result;
}
```
