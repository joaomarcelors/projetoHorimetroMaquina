/*criaçao do banco da dados*/
create database industria
default character set utf8
default collate utf8_general_ci;

/*criaçao da tabela*/
create table industria.maquina1(
	id int not null auto_increment,
    data_inicio_parada date,
    hora_inicio_parada time,
	data_fim_parada date,
    hora_fim_parada time,
    tempo_parado time default (sec_to_time(timestampdiff(second, concat(data_inicio_parada, ' ', hora_inicio_parada), concat(data_fim_parada, ' ', hora_fim_parada)))),
    tipo_parada enum('START/STOP', 'SETUP', 'MANUTENCAO'),
    primary key(id)
)default charset = utf8;

drop table industria.maquina1;

/*testes*/
insert into industria.maquina1 (data_inicio_parada, hora_inicio_parada, data_fim_parada, hora_fim_parada, tipo_parada)
values 
('2020-09-27', '18:00:00', '2020-09-27', '18:14:02', 'MANUTENCAO'),
('2020-09-27', '18:55:44', '2020-09-27', '18:56:30', 'SETUP'),
('2020-09-27', '21:45:00', '2020-09-28', '03:57:21', 'START/STOP');

select *from industria.maquina1 order by id desc;
truncate table industria.maquina1;
