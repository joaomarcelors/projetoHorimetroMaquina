<?php
    $hash = '$2y$10$6K3B6ZxM0iv7niLzd5HIFeOGIZahnkmeDknE3mzYahYQw58CNxkie';

    if(!password_verify($_POST['key'], $hash))
        die("Chave inválida.");

    $data_inicio = $_POST['data_inicio'];
    $hora_incio  = $_POST['hora_inicio'];
    $data_fim = $_POST['data_fim'];
    $hora_fim = $_POST['hora_fim'];
    $tipo = $_POST['tipo'];
    
    if (is_null($data_inicio) || is_null($hora_incio) || is_null($data_fim) || is_null($hora_fim) || is_null($tipo)){
        die("Dados recebidos inválidos");
    }

    $servername = "localhost";
    $username = "root";
    $password = "570137122";
    $dbname = "industria";
    $tabela = "maquina1";
    $conn = new mysqli($servername, $username, $password, $dbname);
    if ($conn->connect_error) {
        die("Não foi possível estabelecer conexão com o BD: " . $conn->connect_error);
    }//else{
       // echo "Conectado ao bando de dados<br/>";
   // } 
    
    $sql = "INSERT INTO industria.$tabela (data_inicio, hora_inicio, data_fim, hora_fim, tipo) 
            VALUES ('$data_inicio', '$hora_incio', '$data_fim', '$hora_fim', '$tipo')";

    //echo "$sql<br/>";
    if (!$conn->query($sql)) {
        die("Erro ao executar query no banco de dados...");
    }else{
        die("OK");
    }
    $conn->close();
?>


