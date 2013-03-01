<?:include bio.p ?>
<?
	$DEF{ "namespace" } = "bbb_sshot_file";
	bio_Start();
	bio_CreateRecord( "hdr", [
			{
				"type"	=> "uint8",
				"name"	=> "magic"
			},
			{
				"type"	=> "uint8",
				"name"	=> "runtime"		# = ( endianess | WORD_SIZE )
			},
			{
				"type"	=> "uint8",
				"name"	=> "platform"
			},
			{
				"type"	=> "uint8",
				"name"	=> "format"			# version of a file format
			}
	] );
	bio_End();
?>
