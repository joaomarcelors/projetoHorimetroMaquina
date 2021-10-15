<?php
    $hash = '$2y$10$6K3B6ZxM0iv7niLzd5HIFeOGIZahnkmeDknE3mzYahYQw58CNxkie';

    if(!password_verify($_POST['key'], $hash))
        die("Chave inválida.");

    $data_inicio_parada = $_POST['data_inicio_parada'];
    $hora_inicio_parada  = $_POST['hora_inicio_parada'];
    $data_fim_parada = $_POST['data_fim_parada'];
    $hora_fim_parada = $_POST['hora_fim_parada'];
    $tipo = $_POST['tipo'];
    
    if (is_null($data_inicio_parada) || is_null($hora_inicio_parada) || is_null($data_fim_parada) || is_null($hora_fim_parada) || is_null($tipo)){
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
    
    $sql = "INSERT INTO industria.$tabela (data_inicio_parada, hora_inicio_parada, data_fim_parada, hora_fim_parada, tipo_parada) 
            VALUES ('$data_inicio_parada', '$hora_inicio_parada', '$data_fim_parada', '$hora_fim_parada', '$tipo')";

    //echo "$sql<br/>";
    if (!$conn->query($sql)) {
        die("Erro ao executar query no banco de dados...");
    }else{
        die("OK");
    }
    $conn->close();
?>


