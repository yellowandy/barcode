<?php
$ext_dir = ini_get('extension_dir');
echo $ext_dir;
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('barcode')) {
	dl('barcode.' . PHP_SHLIB_SUFFIX);
}

$b = scan_image("barcode.png");
print_r($b);
