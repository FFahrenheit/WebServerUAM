<?php 
    echo "Parseo de variables: ";
    echo $argv[1];

    $variables = array();

    $args = explode("&", $argv[1]);
    foreach($args as $arg)
    {
        $vals = explode("=", $arg);
        $variables[$vals[0]] = urldecode($vals[1]);
    }

    print_r($variables);
    print("\r\n");

    print("Unreachable by server");
?>