<?php
   	/**	Livro: PHP5
	 *	Autor: Walace Soares
	 *	Classe: sessao
	 *	Esta classe realiza a manipula��o de sess�es no PHP
	 *	Atributos:
	 *		- id : Cont�m o session_id
	 *		- nvars: Cont�m o tamanho do Array _SESSION
	 *	M�todos Privativos:
	 *		- setNVars: Ajusta $this->_vars
	 *		- setVar:	Cria/Altera uma variavel de Sess�o
	 *					Parametros: _var: Variavel de sess�o
	 *								_valor: valor a ser armazenado
	 *		- unsetVar: Elimina uma variavel de sess�o			  
	 *					Parametro:	_var: Variavel a ser eliminada
	 *		- getVar:	Retorna o valor de uma variavel de sess�o
	 *					Parametro:	_var: Variavel
	 *	M�todos publicos:
	 *		- __construct: Construtor da classe 
	 *						Parametros: _inicia (TRUE ou FALSE)
	 *									_vars (Array com as variaveis e valores)
	 *		- start: Inicia uma sess�o
	 *		- setVars: Cria/Altera uma ou mais variaveis de sess�o
	 *						Parametros:	_var (nome da variavel ou array)
	 *									_valor (valor da variavel, "" se _var for um array)
	 *		- unSetVars: Elimina uma ou v�rias variaveis de sess�o
	 *						Parametros:	_var (nome da variavel ou array)
	 *		- getVars:	Retorna o valor de uma ou mais variaveis de sess�o
	 *						Parametro> _var (nome da variavel ou array)
	 *		- getNVars:	Retorna o tamanho de _SESSION
	 *		- getId:	Retorna o Id da Sess�o Atual	  
	 *		- printAll:	Imprime a rela��o de variaveis da sess�o...
	 *		- destroy:	Exclui a sess�o (session_unset,session_destroy) podendo iniciar uma nova
	 *						Parametros: _inicia (TRUE ou FALSE)
	 *										
	*/
	
	class sessao {
		private $_id;
	 	private $_nvars;

		// Construidor da classe	 
		function __construct($_inicia=FALSE,$_vars=NULL) {
	 		if($_inicia==TRUE) {
				$this->start();	   
				if($_vars!==NULL) {
					$this->setVars($_vars);
				}
			}
		}
		
		// M�todos privativos da classe
	 
		private function setNVars() {
			$this->_nvars = sizeof($_SESSION);	   		
		}

		private function setVar($_var,$_valor) {
			$_SESSION[$_var] = $_valor;
			$this->setNVars();
		}

		private function unSetvar($_var) {
			unset($_SESSION[$_var]);
			$this->setNVars();
		}
		
		private function getVar($_var) {
			if(isset($_SESSION[$_var])) {
		 		return $_SESSION[$_var];
			} else {
				return NULL;
			}
		}
									   
		// M�todos publicos
		
		public function start() {
	   		session_start();
	   		$this->_id = session_id();
			$this->setNVars();
		} 
		
		public function setVars($_var,$_valor="") {
			if(is_array($_var)) {
			 	foreach($_var as $_k=>$_v) {
			    	$this->setVar($_k,$_v);
			 	}
		   	} else {
		   	   	$this->setVar($_var,$_valor);
			}
		}
		
		public function unSetVars($_var) {
			if(is_array($_var)) {
			 	foreach($_var as $_v) {
			    	$this->unsetVar($_v);
			 	}
		   	} else {
		   	   	$this->unsetVar($_var);
			}
		}
		
		public function getVars($_var) {
			if(is_array($_var)) {
				foreach($_var as $_v) {
				   $_res[] = $this->getVar($v);
				}
			} else {
				$_res = $this->getVar($_var);			 	
			}
			
			return $_res;
		}  
		
		public function getNVars() {
			return $this->_nvars;
		}
		
		public function getId() {
			return $this->_id;
		}
		
		public function printAll() {
			foreach($_SESSION as $_k=>$_v) {
			   printf("%s  = %s<br/>",$_k,$_v);
			}
		}
		
		
		public function destroy($_inicia=FALSE) {
			session_unset();
			session_destroy();
			$this->setNVars();
			$this->_id = NULL;
			if($_inicia===TRUE) {
			   $this->start();
			}
		}
		
	}
?>
