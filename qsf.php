<?php
//dl('memcached.so');
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('qsf')) {
	//dl('qsf.' . PHP_SHLIB_SUFFIX);
}
$module = 'qsf';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
$function = 'confirm_' . $module . '_compiled';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled into PHP";
}
echo "$str\n";
echo "ini set:". ini_get("qsf.qsf_int");
echo "\n";
#echo qsf_test("qishengfu", "6666");
echo "\n";
echo qsf_hello("var ");
echo "\n";

$m = new Memcached();
$m->addServer('192.168.0.106', 11211);
$m->set('foo', date('Y-m-d H:i:s'));
#echo $m->get('foo');

$qsfObj = new qsfclass();
//$qsfObj = new qsfclass('174.36.230.90', '21');
$qsfObj->connect('192.168.0.106', "11211");
echo "socket:".$qsfObj->get('foo')."\r\n";
var_dump($qsfObj);
#echo $qsfObj->test("object 's ", "791314");
#echo "\n";
$qsfObj->public_method('127.0.0.1:80');
echo $qsfObj->public_method('174.36.230.90', '21');
#echo $qsfObj->private_method();
#echo $qsfObj->protected_method();
