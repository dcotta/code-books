<?php
/**
 * Configurador de Sistemas existentes no framework
 * N�o incluir o pr�prio Framework
 * O formato deve ser
 * Array('Sistema/M�dulo'=>Diret�rio, ...)
 * Exemplo:
 * 	Array(
 * 			'Estoque/Cadastros'=>'estoque/cadastros/',
 * 			'Estoque/Relat�rios'=>'estoque/relatorios/',
 * 			'Estoque/Processos'=>'estoque/processos/'
 * 		);
 * A chave 'Sistema/M�dulo' ser� exibida na inclus�o de menus
 */
return 	Array('Estoque'=>'estoque/');
?>