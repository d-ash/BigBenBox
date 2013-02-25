<?:include bio.p ?>
<?
	$DEF{ "namespace" } = "test_bio";
	bio_Start();
	bio_CreateRecord( "fileHeader", [
			{
				"type"	=> "uint8",
				"name"	=> "theFirst"
			},
			{
				"type"	=> "uint16",
				"name"	=> "theSecond"
			},
			{
				"type"	=> "uint32",
				"name"	=> "theThird"
			},
			{
				"type"	=> "uint64",
				"name"	=> "theFourth"
			},
			{
				"type"	=> "varbuf",
				"name"	=> "var_buf_777"
			}
	] );
	bio_CreateRecord( "ext333", [
			{
				"type"	=> "varbuf",
				"name"	=> "v1"
			},
			{
				"type"	=> "varbuf",
				"name"	=> "v2"		# = ( endianess | WORD_SIZE )
			}
	] );
	bio_End();
?>
