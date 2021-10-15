/*criaçao do banco da dados*/
create database industria
default character set utf8
default collate utf8_general_ci;

/*criaçao da tabela*/
create table industria.maquina1(
	id int not null auto_increment,
    data_inicio date,
    hora_inicio time,
	data_fim date,
    hora_fim time,
    intervalo_tempo time default (sec_to_time(timestampdiff(second, concat(data_inicio, ' ', hora_inicio), concat(data_fim, ' ', hora_fim)))),
    tipo enum('START/STOP', 'SETUP', 'MANUTENCAO'),
    primary key(id)
)default charset = utf8;


/*testes*/
insert into industria.maquina1 (data_inicio, hora_inicio, data_fim, hora_fim, tipo)
values 
('2020-09-27', '18:00:00', '2020-09-27', '18:14:02', 'MANUTENCAO'),
('2020-09-27', '18:55:44', '2020-09-27', '18:56:30', 'SETUP'),
('2020-09-27', '21:45:00', '2020-09-28', '03:57:21', 'START/STOP');

select *from industria.maquina1 order by id desc;
truncate table industria.maquina1;

drop table industria.maquina1;
drop database industria;
