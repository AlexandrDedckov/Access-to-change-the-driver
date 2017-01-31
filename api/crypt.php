<?

// http://www.industrialnets.ru/files/misc/ascii.pdf

// http://ac-osm.ru1/ajax/db.php?func=fb_routes_search&data%5Btype%5D=full&data%5Bvalue%5D%5B%5D=%D0%B1%D0%B0%D0%BB&data%5Bvalue%5D%5B%5D=(%25+)%3F%3AVALUE%3A%25&data%5Bcenter%5D=59.950%2C30.316

//echo $str = "0123456789:ABCDEFGHIJKLMNOPXYZ[\]^_~!@#$%^&*()_+-/*|?/|`abcdxyz{|}~ `{func=fb_routes_search&data%5Btype%5D=full&data%5Bvalue%5D%5B%5D=%D0%B1%D0%B0%D0%BB&data%5Bvalue%5D%5B%5D=(%25+)%3F%3AVALUE%3A%25&data%5Bcenter%5D=59.950%2C30.316";
//echo $str = "jfunc=fb_routes_search&data[type]=full&data[value][]=балтийс&data[value][]=(% )?:VALUE:%&data[center]=59.950,30.316";
//$enc = "a1p1i1X1xa1W1T1m1j1p1o1Z1n1T1n1Z1V1m1X1c1aY1V1o1V1Zp01o1t1k1Z1Zp31xa1p1g1g1aY1V1o1V1Zp01q1V1g1p1Z1Zp31Zp01Zp31xZ31kZ01lZ31kZ01kZ31kZ0101aY1V1o1V1Zp01q1V1g1p1Z1Zp31Zp01Zp31xcZmpfdZn51Zn&1K1&1A1J141Zn&1ZmpaY1V1o1V1Zp01X1Z1i1o1Z1m1Zp31xptitpkZm21nkinlq";
//echo $enc = encrypt("output=json&udid=3EB5076B-A6CE-4990-852F-E246918E010C&loc=59.848731994629,30.135696411133&loc=59.932697296143,30.337795257568&payment=0");
//echo decrypt($enc);
//enc = encrypt(urlencode($str));
//echo "<br/><b>crypted:</b><br/>" . $enc;
//$dec = decrypt($enc);
//echo "<br/><b>decrypted:</b><br/>" . $dec;
//echo "<br/><b>unescaped:</b><br/>" . urldecode($dec);

// 40-41 ... (-)
// 48-58 ... 0-9: (кроме 49 ... 1)
// 65-90 ... A-Z
// 97-122 ... a-z
// всего 64

// Кодирует символ
function encode_char($ch)
{
	$i = 0; $len = 64; // 3 + 58-49 + 90-64 + 122-96
	if ($ch > $len-1) { $ch = abs($len - $ch); $i++; }
	
	$ar = array("(", ")", "0");
	if ($ch <= 2) $let = $ar[$ch];
	else if ($ch <= 11) $let = chr($ch + 47);
	else if ($ch <= 37) $let = chr($ch - 11 + 64);
	else if ($ch <= 63) $let = chr($ch - 37 + 96);
	
	return array($i, $ch, $let);
}

// Декодирование символа
function decode_char($ch)
{
	$len = 64; // 3 + 58-49 + 90-64 + 122-96;
	$ar = array(40 => 0, 41 => 1, 48 => 2);
	$i = $ch[0]; $ch = ord($ch[1]);
	
	if ($ch >= 97) $ch = $ch + 37 - 96;
	else if ($ch >= 65) $ch = $ch + 11 - 64;
	else if ($ch >= 50) $ch = $ch - 47;
	else if (in_array($ch, array(40, 41, 48))) $ch = $ar[$ch];
	
	return chr($i > 0 ? $ch + 64 : $ch);
}

// Кодирование строки
function encrypt($input)
{
	$output = "";
	$length = mb_strlen($input, "utf8");
	for ($i = 0; $i < $length; $i++)
	{
		$ch = ord($input[$i]);		
		$ch = encode_char($ch);	// символ в нашем списке
		
		//echo "<br/>{$input[$i]} - " . ord($input[$i]) . " - ";
		//print_r($ch);
		
		$output .= $ch[2] . ($ch[0] > 0 ? $ch[0] : "");
	}
	return $output;
}

// Раскодирование строки
function decrypt($input)
{
	//a1p1i1X1xa1W1T1m1j1p1o1Z1n1T1n1Z1V1m1X1c1aY1V1o1V1Zp01o1t1k1Z1Zp31xa1p1g1g1aY1V1o1V1Zp01q1V1g1p1Z1Zp31Zp01Zp31xZ31kZ01lZ31kZ01kZ31kZ0101aY1V1o1V1Zp01q1V1g1p1Z1Zp31Zp01Zp31xcZmpfdZn51Zn:1K1:1A1J141Zn:1ZmpaY1V1o1V1Zp01X1Z1i1o1Z1m1Zp31xptitpkZm21nkinlq

	$input .= "#";
	$output = "";
	$chs = array();
	
	$length = mb_strlen($input, "utf8");
	$offset = 0;
	
	// парсим строку на символы
	while ($offset < $length - 1)
	{
		$ch = mb_strcut($input, $offset, 2, "utf8");
		if ($ch[1] == "1") $offset += 1;
		else $ch[1] = "0";
		
		$chs[] = array(intval($ch[1]), $ch[0]);
		$offset += 1;
	}

	// расшифровываем символы
	for ($i = 0; $i < count($chs); $i++)
	{
		$ch = $chs[$i];		
		$ch_dec = decode_char($ch);	// символ в нашем списке
		
		//echo "<br/>{$ch_dec} - ";
		//print_r($ch);
		
		$output .= $ch_dec;
	}

    $file = 'c:/Progect QT/test.txt';
    file_put_contents($file, "");
    $current = file_get_contents($file);
    file_put_contents($file, $current."\n" .$output);

	return $output;
}

?>