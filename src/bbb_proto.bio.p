<?:include bio.p ?>
<?
	$DEF{ "namespace" } = "bbb_proto";
	bio_Start();

	bio_CreateRecord( "hdr", [
		{
			"type"	=> "uint16",
			"name"	=> "type"
		},
		{
			"type"	=> "uint16",	# length of the body
			"name"	=> "length"	
		},
		{
			"type"	=> "uint32",	# checksum of the body
			"name"	=> "checksum"	
		}
	] );

	bio_CreateRecord( "msgAuth", [
		{
			"type"	=> "varbuf",
			"name"	=> "login"
		},
		{
			"type"	=> "varbuf",	# TODO
			"name"	=> "password"	# TODO
		}
	] );

	bio_End();
?>
