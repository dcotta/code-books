<?php
/**
 * Classe Base do framework, dever� ser estendida nas classes filhas
 *
 */
class BASE {
	protected $_nome_tabela;
	protected $_campos = Array();
	protected $_msgextra = "";
	/**
	 * @var BancoDados
	 */
	protected $_conn = null;
	protected $_class_path = "framework";
	protected $_exibe_opcoes = true;	// Define se a classe tem as op��es de inc/exc/alt
	
	public function __construct(BancoDados &$_conn=null) {
		$this->_conn = $_conn;
	}
	
	public function addCampo(campo $_campo) {
		$this->_campos[$_campo->getNome()] = $_campo;
		$_campo->setArquivo_Classe("{$this->_class_path}/classes/classe_" . get_class($this) . ".inc");
		$_campo->setClasse(get_class($this));
	}
	
	public function deleteCampo($_campo) {
		unset($this->_campos[$_campo]);
	}
	
	public function clearCampos() {
		$this->_campos = Array();
	}
	
	public function getCampo($_campo) {
		return $this->_campos[$_campo];
	}
	
	public function getListaCampos() {
		return array_keys($this->_campos);
	}
	
	public function filtrarCampos($_filtro="listar",$_valor=true) {
		$_campos = Array();
		foreach($this->_campos as $_campo) {
			$_fcn = "get{$_filtro}";
			if($_campo->$_fcn()===$_valor) {
				$_campos[$_campo->getNome()] = $_campo;
			}
		}
		return $_campos;
	}
	
	public function montaSELECT($_campos,$_where=null,$_orderby=null,$_limit=null,$_extras=null) {
		$_strSQL = "SELECT {$_campos} FROM {$this->_nome_tabela} " . 
					($_where!==null ? "WHERE {$_where}" : "") .
					($_orderby!==null ? " ORDER BY {$_orderby}" : "") . 
					($_extras!==null ? " {$_extras}" : "") . 
					($_limit!==null ? " LIMIT {$_limit}" : ""); 
		return $_strSQL;
	}
	
	/**
	 * Define o modo de ordena��o dos registros
	 * O modo padr�o � a chave prim�ria em ordem ascendente
	 *
	 * @return string
	 */
	public function getOrdem() {
		return implode(",",array_keys($this->filtrarCampos("pk")));
	}
	
	public function listar() {
		$_camposlista = array_merge($this->filtrarCampos(),$this->filtrarCampos("pk")); // Campos que ser�o exibidos na listagem
		if(($_filtro=$this->getFiltro())!==null) {
			$_filtro_html = utf8_decode(implode(", ",$_filtro["HTML"]));
			$_filtro = implode(" AND ",$_filtro["SQL"]);
		}
		// 1a. Tarefa, buscar o Total de Registros, mas somente na primeira VEZ...
		if(!isset($_GET['pagina'])) {
			$_strSQL = $this->montaSELECT("COUNT(*) AS TOTAL",$_filtro);
			$_total_registros = 0;
			if($this->_conn->executaSQL($_strSQL)!==false) {
				$_dados = $this->_conn->proximo();
				$_total_registros = $_dados['TOTAL'];
			}
		} else {
			$_total_registros = $_GET['total_registros'];
		}
		$_limit = $this->_conn->getLimit(paginacao::getLimit(isset($_GET['pagina']) ? $_GET['pagina'] : 1)); 
		$_strSQL = $this->montaSELECT(implode(",",array_keys($_camposlista)),$_filtro,$this->getOrdem(),$_limit);
		$_tipos = new tipospadrao();
		$_tab = new tag($_tipos->getTipo('TABLE'),Array(new atributo("BORDER",0),
														new atributo("CELLPADDING",5),
														new atributo("STYLE","border:1px solid #a0a0a0;border-collapse:collapse;"),
														new atributo("WIDTH","100%")));
		$_tr  = new tag($_tipos->getTipo('TR'));
		foreach($_camposlista as $_campo) {
			$_tr->addSubTag(new tag($_tipos->getTipo('TH'),Array(new atributo("STYLE","border:1px solid #a0a0a0;background-color:#f0f0f0;")),
									$_campo->getTitulo()));
		}
		$_numcols = sizeof($_camposlista);
		if($this->_exibe_opcoes===true) {
			$_tr->addSubTag(new tag($_tipos->getTipo('TH'),Array(new atributo("STYLE","border:1px solid #a0a0a0;background-color:#f0f0f0;")),"Op��es"));
			++$_numcols;
		}
		$_tab->addSubTag($_tr);
		if($this->_conn->executaSQL($_strSQL)!==false&&$this->_conn->getNumRows()>0) {
			while($this->proximo()) {
				$_tab->addSubtag(new tag($_tipos->getTipo('TR'),
								Array(new atributo("ONMOUSEOVER","this.className='detalhe';"),
									  new atributo("ONMOUSEOUT","this.className='';"))));
				$_pk = "";
				foreach($_camposlista as $_campo) {
					if($_campo->getPK()===true) {
						$_pk .= "&{$_campo->getNome()}={$_campo->toHTML()}";
					}
					$_tab->getLastSubTag()->addSubTag(new tag($_tipos->getTipo('TD'),
												  Array(new atributo("STYLE","border:1px solid #a0a0a0;"),$_campo->getAtributosExtras()),
												  $_campo->toHTML()));
				}
				if($this->_exibe_opcoes===true) {
					$_lnkalt = new tag($_tipos->getTipo("A"),Array(new atributo("HREF","javascript:void(0);"),
																   new atributo("ONCLICK","ObjProcAjax.run('{$_SERVER['PHP_SELF']}?ACAO=ALT{$_pk}','CORPO');"))," Alterar ");
					$_lnkexc = new tag($_tipos->getTipo("A"),Array(new atributo("HREF","javascript:void(0);"),
																   new atributo("ONCLICK","ObjProcAjax.run('{$_SERVER['PHP_SELF']}?ACAO=EXC{$_pk}','CORPO');"))," Excluir ");
					$_tab->getLastSubTag()->addSubTag(new tag($_tipos->getTipo('TD'),
												  Array(new atributo("STYLE","border:1px solid #a0a0a0;")),$_lnkalt->toHTML() . $_lnkexc->toHTML()));
				}
			}
			$_trf = new tag($_tipos->getTipo('TR'));
			$_trf->addSubTag(new tag($_tipos->getTipo("TD"),Array(new atributo("COLSPAN",$_numcols),
																  new atributo("STYLE","color:#a0a0a0;font-size:11px;border:1px solid #a0a0a0;")),
									 "Total de {$_total_registros} Registro(s)"));
			$_tab->addSubTag($_trf);
		} else {
			$_trf = new tag($_tipos->getTipo('TR'));
			$_trf->addSubTag(new tag($_tipos->getTipo("TD"),Array(new atributo("COLSPAN",$_numcols),
																  new atributo("STYLE","border:1px solid #a0a0a0;text-align:center")),
									 "Nenhum Registro"));
			$_tab->addSubTag($_trf);
			
		}
		$_html = "";
		$_div = new tag($_tipos->getTipo('DIV'));
		$_div->addSubTag($_tab);
		if($_filtro_html!==null) {
			$_div->addSubTag(new tag($_tipos->getTipo("DIV"),Array(new atributo("STYLE","padding:10px;color:#c0c0c0;font-size:12px;")),"Filtro Aplicado: {$_filtro_html}"));
		}
		$_pag = new tag($_tipos->getTipo("P"),Array(new atributo("ALIGN","CENTER")));
		$_pag->setValor(implode(" ",paginacao::paginar($_total_registros,(isset($_GET['pagina']) ? $_GET['pagina'] : 1),null,true)));
		$_div->addSubTag($_pag);
		if($this->_exibe_opcoes===true) {
			$_div->addSubTag(new tag($_tipos->getTipo("BR")));
			$_div->addSubTag(new tag($_tipos->getTipo("P"),Array(new atributo("ALIGN","CENTER"))));
			$_div->getLastSubTag()->addSubTag(new tag($_tipos->getTipo('BUTTON'),
												Array(new atributo("ONCLICK","ObjProcAjax.run('{$_SERVER['PHP_SELF']}?ACAO=INC','CORPO');")),
												" Incluir Novo Registro "));
		}
		$_html .= $_div->toHTML();
		return utf8_encode($_html);
	}	
	
	public function preenche($_d) {
		foreach($this->_campos as $_k=>$_campo) {
			$this->_campos[$_k]->SetValor($_d[$_k]);
		}
	}
	
	public function proximo()  {
		if(($_d=$this->_conn->proximo())!==false) {
			$this->preenche($_d);
			return true;
		} else {
			return false;
		}
	}
	
	/**
	 * Busca registros no banco de dados
	 *
	 * @param array $_valores
	 */
	public function Buscar($_valores=null) {
		$_pk = $this->filtrarCampos("pk");
		$_where = null;
		if($_valores!==null) {
			// Gerar a clasula where
			$_where = "";
			foreach($_pk as $_nome=>$_campo) {
				if(isset($_valores[$_nome])) {
					$_campo->setValor($_valores[$_nome]);
					$_where .= ($_where!='' ? " AND " : "") . "{$_nome}=" . $_campo->toBD();
				}
			}
		}
		if($this->_conn->executaSQL($this->montaSELECT("*",$_where,implode(",",array_keys($_pk))))===false) {
			return false;
		} else {
			return $this->_conn->getNumRows();
		}
	}
	
	/**
	 * Inclui um registro na tabela relacionada
	 *
	 * @return mixed (false = erro)
	 */
	public function incluir() {
		$_listacampos = Array();
		$_listavalores = Array();
		foreach($this->filtrarCampos("incluir") as $_campo) {
			if($_campo->getValor()!==null) {
				$_listacampos[]  = $_campo->getNome();
				$_listavalores[] = $_campo->toBD();
			}
		}
		$_strSQL = "INSERT INTO {$this->_nome_tabela} (" . 
					implode(",",$_listacampos) . ") VALUES(" . 
					implode(",",$_listavalores) . ")";
		if($this->_conn->executaSQL($_strSQL)!==false) {
			$_res = $this->_conn->getNumRows();
			if(!($this instanceof auditoria||$this instanceof historico||$this instanceof favoritos)) {
				$_audit = new auditoria($this->_conn);
				$_audit->getCampo("CLASSE")->setValor(get_class($this));
				$_audit->getCampo("FUNCAO")->setValor('INC');
				$_audit->getCampo("HISTORICO")->setValor($_strSQL);
				$_audit->incluir();
			}
			return $_res;
		} else {
			return false;
		}
	}
	
	/**
	 * Altera um registro na tabela relacionada
	 *
	 * @return mixed (false = erro)
	 */
	public function alterar() {
		$_listacampos = "";
		$_listachave = "";
		$_pk = $this->filtrarCampos("pk");
		foreach($_pk as $_nome=>$_campo) {
			$_listachave .= ($_listachave!="" ? " AND " : "") . "{$_nome}=" . $_campo->toBD();
		}
		foreach($this->filtrarCampos("alterar") as $_nome=>$_campo) {
			if($_campo->getValor()!==null&&!key_exists($_nome,$_pk)) {
				$_listacampos .= ($_listacampos!="" ? "," : "") . "{$_nome}=" . $_campo->toBD();
			}
		}
		$_strSQL = "UPDATE {$this->_nome_tabela} SET {$_listacampos} WHERE {$_listachave}";
		if($this->_conn->executaSQL($_strSQL)!==false) {
			$_res = $this->_conn->getNumRows();
			if(!($this instanceof historico)) {
				$_audit = new auditoria($this->_conn);
				$_audit->getCampo("CLASSE")->setValor(get_class($this));
				$_audit->getCampo("FUNCAO")->setValor('ALT');
				$_audit->getCampo("HISTORICO")->setValor($_strSQL);
				$_audit->incluir();
			}
			return $_res;
		} else {
			return $this->_conn->getUltimoErro();
		}
		
	}
	
	/**
	 * Exclui um Registro (somente 1)
	 *
	 * @return mixed (false = erro)
	 */
	public function excluir() {
		$_listachave = "";
		$_pk = $this->filtrarCampos("pk");
		foreach($_pk as $_nome=>$_campo) {
			$_listachave .= ($_listachave!="" ? " AND " : "") . "{$_nome}=" . $_campo->toBD();
		}
		$_strSQL = "DELETE FROM {$this->_nome_tabela} WHERE {$_listachave}";
		if($this->_conn->executaSQL($_strSQL)!==false) {
			$_res = $this->_conn->getNumRows();
			if(!($this instanceof auditoria||$this instanceof historico||$this instanceof favoritos)) {
				$_audit = new auditoria($this->_conn);
				$_audit->getCampo("CLASSE")->setValor(get_class($this));
				$_audit->getCampo("FUNCAO")->setValor('INC');
				$_audit->getCampo("HISTORICO")->setValor($_strSQL);
				$_audit->incluir();
			}
			return $_res;
		} else {
			return $this->_conn->getUltimoErro();
		}
	}
	
	public function getConn() {
		return $this->_conn;
	}
	
	/**
	 * Valida um Valor para Filtro
	 * Deve ser implementado na classe filha
	 */
	protected function validaValor($_campo,$_valor) {
		return true;
	}
	
	/**
	 * Grava o Filtro na Sess�o do usu�rio
	 *
	 */
	public function salvarFiltro() {
		//Geramos um filtro na Sess�o Atual
		$_filtro = Array();
		foreach($this->filtrarCampos("filtro") as $_chv=>$_campo) {
			if(!empty($_POST[$_chv])) {
				if($this->validaValor($_chv,$_POST[$_chv])) {
					$_campo->setValor($_POST[$_chv]);
					$_filtro["SQL"][]  	= "{$_chv}={$_campo->toBD()}";
					$_filtro["HTML"][] 	= utf8_encode("{$_campo->getTitulo()}={$_campo->getValor()}");
					$_filtro[$_chv] 	= $_POST[$_chv]; //$_campo->toBD();
				}
			}
		}
		$_SESSION["FILTRAR_" . get_class($this)] = serialize($_filtro);
	}
	
	/**
	 * Retorna os filtros de uma classe
	 *
	 * @return mixed	Array ou null
	 */
	public function getFiltro() {
		$_filtro = $_SESSION["FILTRAR_" . get_class($this)];
		if($_filtro!==null&&!empty($_filtro)) {
			$_arr = unserialize($_filtro); 
			return (sizeof($_arr)>0 ? $_arr : null);
			
		} else {
			return null;
		}
	}
	
	public function processaAcao() {
		if(isset($_POST['metodo'])) {
			$this->{$_POST['metodo']}();
		}
		$_comandos  = "document.getElementById('TITULO').innerHTML='{$GLOBALS['_FTitulo']}';";
		$_comandos .= "ObjInfoPagina.classeAtual='" . get_class($this) . "';";
		$_comandos .= "ObjInfoPagina.arquivoClasse='{$this->_class_path}/classes/classe_" . get_class($this) . ".inc';";
		$_comandos .= "ObjInfoPagina.programaAtual='{$_SERVER['PHP_SELF']}';";
		$_sct    = new tag(new tipotag("SCRIPT"),null,$_comandos);
		if($_GET['ACAO']!==null) {
			$_ok = true;
			if($_GET['ACAO']!=='INC') {
				foreach($_GET as $_k=>$_v) {
					if($_k!='ACAO') {
						$_valores[$_k] = $_v;
					}
				}
				$_res = $this->Buscar($_valores);
				if($_res!==false&&$_res==1) {
					$this->proximo();
				} else {
					$_scr = new tag(new tipotag("SCRIPT"),null,"alert('Registro informado n�o foi encontrado');");
					$_html = utf8_encode($_scr->toHTML());
					$_ok = false;
				}
			}
			if($_ok===true) {
				echo $this->getFormulario($_GET['ACAO']);
				return;
			}
		} elseif($_POST['ACAO']==='SALVAR') {
			foreach($this->_campos as $_nome=>$_campo) {
				$_campo->SetValor(isset($_POST[$_nome]) ? $_POST[$_nome] : null);
			}
			switch($_POST['FUNCAO']) {
				case 'ALT':	$_msg = $this->alterar()!==false 
										? "Registro Alterado [{$this->getConn()->getNumRows()}]" 
										: "Erro na Altera��o: {$this->getConn()->getUltimoErro()}";
							break;
				case 'INC':	$_msg = $this->incluir()!==false 
										? "Registro Incluido [{$this->getConn()->getNumRows()}]" 
										: "Erro na Inclus�o: {$this->getConn()->getUltimoErro()}";
							break;
				case 'EXC':	$_msg = $this->excluir()!==false
										? "Registro Excluido [{$this->getConn()->getNumRows()}]" 
										: "Erro na Exclus�o: {$this->getConn()->getUltimoErro()}";
				
							break;
				default:	$_msg= "Fun��o {$_POST['FUNCAO']} N�o Implementada<br><BR><BR>";	
			}
			$_msg .= " {$this->_msgextra}";
			return "alert('". str_replace("'","\\'",str_replace("\n"," ",$_msg)) . "');{$_titulo}";
		}
		echo $_html . $this->listar() . utf8_encode($_sct->toHTML());
	}
	
	public function getFormulario($_funcao="INC") {
		$_fcn  = ($_funcao!='ALT' ? "incluir" : 'alterar');
		$_form = new formulario("FRM_{$this->_nome_tabela}","index.php5");
		$_form->addSubTag(new tag(new tipotag("SCRIPT"),null,"objForm.construct();"));
		$_form->addSubTag(new formInputHidden("SCRIPT_NAME","{$_SERVER['PHP_SELF']}"));
		$_form->addSubTag(new formInputHidden("ACAO","SALVAR"));
		$_form->addSubTag(new formInputHidden("FUNCAO",$_funcao));
		if($_funcao!='INC') {
			// Devemos gravar as chaves
			foreach($this->filtrarCampos("pk") as $_pk) {
				$_form->addSubTag(new formInputHidden($_pk->getNome(),$_pk->getValor()));
			}
			if($_funcao=='ALT') {
				// Vamos gerar os valores Atuais dos campos
				foreach($this->filtrarCampos("alterar") as $_campo) {
					$_form->addSubTag(new formInputHidden("{$_campo->getNome()}_OLD",$_campo->toHTML()));
				}
			}
		}
		$_tab  = new tag(new tipotag("TABLE"),
						 Array(new atributo("BORDER",0),new atributo("CELLPADDING",2),new atributo("CELLSPACING",0)));
		$_tr = new tipotag("TR");
		$_td = new tipotag("TD");
		// fcn=ALT
		if($_funcao=='ALT') {
			// Vamos listar as chaves
			foreach($this->filtrarCampos("pk") as $_pk) {
				$_tab->addSubTag(new tag($_tr));
				$_tab->getLastSubTag()->addSubTag(new tag($_td,Array(new atributo("STYLE","color:Navy;width:200px;")),"{$_pk->getTitulo()}:"));
				$_tab->getLastSubTag()->addSubTag(new tag($_td));
				$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new tag(new tipotag("SPAN"),Array(new atributo("STYLE","color:#ff0011;")),$_pk->toHTML()));
			}
		}
		foreach($this->filtrarCampos($_fcn) as $_campo) {
			$_tab->addSubTag(new tag($_tr));
			$_tab->getLastSubTag()->addSubTag(new tag($_td,Array(new atributo("STYLE","color:Navy;width:200px;")),"{$_campo->getTitulo()}:"));
			$_tab->getLastSubTag()->addSubTag(new tag($_td));
			// fcn=Exc
			if($_funcao=='EXC') {
				$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new tag(new tipotag("SPAN"),Array(new atributo("STYLE","color:#ff0011;")),$_campo->toHTML()));
			} else {
				if(strtolower($_campo->getComportamento_form())=='radio'&&is_array($_campo->getValor_fixo())) {
					foreach($_campo->getValor_Fixo() as $_opcao) {
						$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new formInputRadio($_campo->getNome(),
																			$_opcao['valor'],$_opcao['label'],null,null,
																			($_opcao['marcar']===true||$_campo->getValor()==$_opcao['valor'])));
						$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new tag(new tipotag('BR',false)));
					}
				} else {
					$_tab->getLastSubTag()->getLastSubTag()->addSubTag($_campo->toForm());
				}
				if(strtolower($_campo->getComportamento_form())=='file') {
					$_form->addAtributo(new atributo("ENCTYPE","multipart/form-data"));
				}
			}
		}
		$_tab->addSubTag(new tag($_tr));
		$_tab->getLastSubTag()->addSubTag(new tag($_td,Array(new atributo("COLSPAN",2),new atributo('ALIGN','CENTER'),new atributo("ID","BTNOK"))));
		$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new formInputSubmit("OK",($_funcao=='INC' ? ' Incluir ' : ($_funcao=='ALT' ? ' Alterar' : ' Confirmar Exclus�o '))));
		$_form->addSubTag($_tab);
		return utf8_encode($_form->toHTML(false));
	}
	
	
	/**
	 * Este m�todo n�o faz nada, � necess�rio implementa-lo
	 * na Classe para que seja retorna o SQL de pesquisa
	 *
	 * @return Array||null
	 */
	public function montaSQLAjax() {
		return null;
	}
	
	/**
	 * M�otodo gen�rico para retornar dados de uma consulta
	 * Ajax
	 *
	 */
	public function buscaDadosAjax() {
		if(($_strSQL=$this->montaSQLAjax())===null) {
			return Array(0=>Array("valor"=>0,"label"=>"N�o Implementado"));
		} else {
			$_opcoes = Array();
			if($this->_conn->executaSQL("{$_strSQL} LIMIT 26")!==false&&$this->_conn->getNumRows()>0) {
				while(($_dados=$this->_conn->proximo())!==false) {
					$_opcoes[] = Array('valor'=>$_dados['CODIGO'],'label'=>$_dados['DESCRICAO']);
				}
			}
			$this->retornaListAjax($_opcoes);
		}
	}
	
	/**
	 * Retorna a lista em uma consulta Ajax
	 * no formato estabelecido pelo script.aluco.us
	 *
	 * @param Array $_lista
	 */
	public function retornaListAjax(Array $_lista) {
		$_strlista = "";
		if(sizeof($_lista)>0) {
			$_c=0;
			foreach($_lista as $_opc) {
				$_strlista .= "<li valor=\"{$_opc['valor']}\" target=\"{$_POST['target']}\">{$_opc['label']}</li>";
				++$_c;
				if($_c>=25) {
					$_strlista .= "<li valor=\"0\" target=\"{$_POST['target']}\">
									<span style='color:#c0c0c0;font-size:10px;'>** primeiros 25 registros **</span></li>";
					break;
				}
			}
		} else {
			$_strlista = "<li valor=\"0\" target=\"{$_POST['target']}\">* Nenhum Registro *</li>";
		}
		echo utf8_encode("<ul>{$_strlista}</ul>");
	}
	
	/**
	 * Retorna o comando SQL necess�rio para
	 * a busca do pr�ximo c�digo
	 * Deve ser implementado na classe filha
	 *
	 * @return string
	 */
	public function montaSQLProximoCodigo() {
		return null;
	}
	
	/**
	 * Retorna o pr�ximo c�digo disponpivel
	 *
	 * @return mixed
	 */
	public function getProximoCodigo($_return=false) {
		if(($_strSQL=$this->montaSQLProximoCodigo())!==null) {
			if($this->_conn->executaSQL($_strSQL)!==false&&$this->_conn->getNumRows()>0) {
				$_dados=$this->_conn->proximo();
				if($_dados!==false)	{
					$_res = ((int) $_dados['CODIGO'])+1;
				}
				
			} else {
				$_res = 1;
			}
		} else {
			$_res = "[ERRO]";
		}
		if($_return===true) {
			return $_res;
		} else {
			echo $_res;
		}
	}
	
	
	/**
	 * Processamento da classe antes do Filtro
	 * Deve ser implementado na classe Filha
	 */
	protected function preFiltro() {
	}
	
	/**
	 * Retorna HTML informando que o filtro n�o 
	 * � suportado pela classe
	 * Deve ser chamado explicitamente pela classe
	 *
	 */
	protected function filtroNaoSuportado() {
		$_aviso = new tag(new tipotag("DIV"),Array(new atributo("STYLE","text-align:center;color:#DC143C;padding:10px;font-weight:bold;")),"Filtro n�o suportado");
		$_aviso->addSubTag(new tag(new tipotag("SCRIPT"),null,"setTimeout('processaFiltro()',4000);"));		
		echo utf8_encode($_aviso->toHTML());
	}
	
	/**
	 * Gera o formul�rio para filtro de conte�do
	 *
	 * @return mixed
	 */
	public function getFormularioFiltro() {
		$this->preFiltro();
		$_filtro = $this->getFiltro();
		$_form = new formulario("FILTRO_{$this->_nome_tabela}","Javascript:void(0);");
		$_form->deleteAtributo(4);
		$_form->addAtributo(new atributo("ONSUBMIT","ObjProcAjax.runPost('{$_POST['programa']}','CORPO',retornaFiltro(this));processaFiltro();return false;"));
		$_tab  = new tag(new tipotag("TABLE"),
						 Array(new atributo("BORDER",0),new atributo("CELLPADDING",2),
						 	   new atributo("CELLSPACING",0),new atributo("WIDTH","100%")));
		$_tr = new tipotag("TR");
		$_td = new tipotag("TD");
		$_tab->addSubTag(new tag($_tr));
		$_tab->getLastSubTag()->addSubTag(new tag($_td,
							Array(new atributo("COLSPAN",4),
								  new atributo("STYLE","text-align:left;font-weight:bold;color:#ffffff;background-color:#aaccff;border-bottom:1px solid #c0c0c0;padding-left:10px;")),"F i l t r o"));
		$_tab->addSubTag(new tag($_tr));
		$_tab->getLastSubTag()->addSubTag(new tag($_td,
							Array(new atributo("COLSPAN",4),
								  new atributo("STYLE","height:10px;font-size:1px;")),"&nbsp;"));
		// Colunas ...
		$_colunas = 3;
		foreach($this->filtrarCampos('filtro') as $_chv=>$_campo) {
			if($_colunas>2) {
				$_tab->addSubTag(new tag($_tr));
				$_colunas = 1;
			}
			++$_colunas;
			if(isset($_filtro[$_chv])) {
				$_campo->setValor(utf8_decode($_filtro[$_chv]));
			}
			$_tab->getLastSubTag()->addSubTag(new tag($_td,Array(new atributo("STYLE","color:Navy;width:200px;padding:5px;")),"{$_campo->getTitulo()}:"));
			$_tab->getLastSubTag()->addSubTag(new tag($_td));
			if(strtolower($_campo->getComportamento_form())=='radio'&&is_array($_campo->getValor_fixo())) {
				foreach($_campo->getValor_Fixo() as $_opcao) {
					$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new formInputRadio($_campo->getNome(),
																		$_opcao['valor'],$_opcao['label'],null,null,
																		$_opcao['marcar']===true||$_campo->getValor()==$_opcao['valor']));
					$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new tag(new tipotag('BR',false)));
				}
			} else {
				$_tab->getLastSubTag()->getLastSubTag()->addSubTag($_campo->toForm());
			}
		}
		$_tab->addSubTag(new tag($_tr));
		$_tab->getLastSubTag()->addSubTag(new tag($_td,Array(new atributo("COLSPAN",4),new atributo('STYLE','text-align:right;padding-right:20px;'),new atributo("ID","BTNOK"))));
		$_tab->getLastSubTag()->getLastSubTag()->addSubTag(new formInputSubmit("OK"," Aplicar Filtro "));
		$_form->addSubTag($_tab);
		echo utf8_encode($_form->toHTML(false));
	}
	
	
	/**
	 * Gera o Formul�rio para Relat�rios do Sistema
	 *
	 */
	public function getFormularioRelatorio() {
		$_rel = new formularioRelatorio($this);
		$_rel->getFormularioRelatorio();
	}
	
	/**
	 * Imprime o Relat�rio
	 *
	 */
	public function imprimirRelatorio() {
		$_rel = new relatorio($this);
		echo $_rel->Imprimir();
	}
	
	/**
	 * Pr�-Processamento dos Dados do Relat�rio
	 * Deve ser implementado na classe atual
	 */
	public function processaRelatorio() {
	}
	
}

/**
 * Classe abstrata para gerenciamento de Campos do framework
 *
 */
abstract class campo {
	protected $_nome;
	protected $_titulo;
	protected $_tipo;
	protected $_tamanho;
	protected $_formato;
	protected $_relacao;
	protected $_incluir;
	protected $_alterar;
	protected $_listar;
	protected $_minimo;
	protected $_maximo;
	protected $_camporef;
	protected $_valor;
	protected $_valor_anterior;
	protected $_pk;
	protected $_valor_fixo=null;
	protected $_marcar=false;
	protected $_comportamento_form=null;
	protected $_url_ajax = 'executa_busca_ajax.php5';
	protected $_ajax_params_extras = '';
	protected $_classe = '';
	protected $_arquivo_classe = '';
	protected $_obrigatorio = true;
	protected $_filtro = true; // Exibe o campo no filtro (default=sim)
	protected $_relatorio = true; // Exibe o cmapo no relat�rio (default=sim)
	/**
	 * @var BancoDados $_conn
	 */
	protected $_conn = null;
	
	abstract public function toBD();
	abstract public function toHTML();
	
	/**
	 * Construtor da classe
	 *
	 * @param string  $_nome
	 * @param string  $_titulo
	 * @param string  $_tipo
	 * @param integer $_tamanho
	 * @param string  $_formato
	 * @param string  $_relacao
	 * @param bool    $_incluir
	 * @param bool 	  $_alterar
	 * @param bool    $_listar
	 * @param string  $_minimo
	 * @param string  $_maximo
	 * @param string  $_camporef
	 * @param bancodado $_conn
	 */
	public function __construct($_nome,$_titulo,$_tipo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false,bancodados &$_conn=null) {
		$this->_nome 		= $_nome;
		$this->_tipo		= $_tipo;
		$this->_titulo 		= $_titulo;
		$this->_tamanho 	= $_tamanho;								
		$this->_formato		= $_formato;
		$this->_relacao		= $_relacao;
		$this->_incluir		= (bool) $_incluir;
		$this->_alterar		= (bool) $_alterar;
		$this->_listar		= (bool) $_listar;
		$this->_minimo		= $_minimo;
		$this->_maximo		= $_maximo;
		$this->_camporef	= $_camporef;
		$this->_valor		= null;
		$this->_conn		= $_conn;
		$this->_pk			= (bool) $_pk;
	}
	
	public function getAtributosExtras() {
		return new atributo("ALIGN","LEFT");
	}
	
	/* Fun��o Gen�rica para Chamada de m�todos inexistentes na classe */
	/* Formato do Nome: [Set|Get][Nome_Atributo]([Valor]) */
	public function __call($_metodo,$_valores) {
		$_m = strtolower($_metodo);
		switch (substr($_m,0,3))  {
			case 'get':
			case 'set':	$_atributo = "_" . substr($_m,3);
						if(property_exists($this,$_atributo)) {
							if(substr($_m,0,3)=='set') {
								$this->{$_atributo} = $_valores[0];
								return True;
							} else {
								return $this->{$_atributo};
							}
						} else {
							return false;
						}
						break;
		}
	}
	
	public function toForm() {
		switch(strtolower($this->_comportamento_form)) {
		case 'password': $_cpofrm = new formInputPassword($this->_nome,$this->_tamanho>30 ? 30 : $this->_tamanho,$this->_tamanho);
					 	 $_cpofrm->setValidaCampo($this->_tipo,$this->_nome,$this->_titulo,$this->_minimo,$this->_maximo,$this->_obrigatorio);						 
						 break;
		case 'file':	 $_cpofrm = new formInputFile($this->_nome,$this->_tamanho>30 ? 30 : $this->_tamanho,$this->_tamanho);
						 break;
		case 'checkbox': $_cpofrm = new formInputCheckBox($this->_nome,$this->_valor_fixo,null,null,null,($this->_valor==$this->_valor_fixo)||$this->_marcar);
						 break;
		case 'radio':	 $_cpofrm = new formInputRadio($this->_nome,$this->_valor_fixo,null,null,null,($this->_valor==$this->_valor_fixo)||$this->_marcar);
						 break;
		case 'select':	 $_cpofrm = new formSelect($this->_nome,false,is_array($this->_valor_fixo) ? $this->_valor_fixo : Array());
						 if($this->_valor!==null) {
						 	$_cpofrm->marcarOpcao($this->_valor);
						 }
						 break;
		case 'mselect':	 $_cpofrm = new formSelect("{$this->_nome}[]",true,is_array($this->_valor_fixo) ? $this->_valor_fixo : Array());
						 if(is_array($this->_valor)) {
						 	foreach($this->_valor as $_valor) {
						 		$_cpofrm->marcarOpcao($_valor);
						 	} 
						 } elseif($this->_valor!==null) {
						 	$_cpofrm->marcarOpcao($this->_valor);
						 }
						 break;
		case 'textarea': $_cpofrm = new formTextArea($this->_nome);
						 $_cpofrm->setValor($this->toHTML());
						 break;
		default: 		 $_cpofrm = new formInputTexto($this->_nome,$this->_tipo,$this->_tamanho>30 ? 30 : $this->_tamanho,$this->_tamanho);
						 // Ajax.Autocompleter
						 if(strtolower($this->_comportamento_form)=='ajax') {
						 	$_cpofrm->addSubTag(new tag(new tipotag("SPAN"),
						 							Array(new atributo("ID","aguarde_{$this->_nome}"),new atributo("STYLE","display:none;"))));
							$_cpofrm->getLastSubTag()->addSubTag(new tag(new tipotag("IMG",false),
													Array(new atributo("SRC","framework/imagens/spinner.gif"),new atributo("ALIGN","ABSMIDDLE"))));
						 	$_cpofrm->addSubTag(new tag(new tipotag("DIV"),
						 							Array(new atributo("ID","resultado_{$this->_nome}"),new atributo("CLASS","autocomplete"))));
						 	$_cpofrm->addSubTag(new tag(new tipotag("SCRIPT"),null,"new Ajax.Autocompleter('{$this->_nome}'," . 
						 	 						"'resultado_{$this->_nome}', '{$this->_url_ajax}', {paramName: 'valor'," . 
  													"minChars: 2, updateElement: atualizaFormulario, indicator: 'aguarde_{$this->_nome}'," .
  													"parameters: 'target={$this->_nome}&classe={$this->_classe}&" . 
  													"arquivo_classe={$this->_arquivo_classe}{$this->_ajax_params_extras}'});"));
						 } else {
							$_cpofrm->setVerificaCampo($this->_tipo);						 
						 }
						 $_cpofrm->setValidaCampo($this->_tipo,$this->_nome,$this->_titulo,$this->_minimo,$this->_maximo,$this->_obrigatorio);						 
						 // Busca pr�ximo c�digo
						 if(strtolower($this->_comportamento_form)=='proximo') {
							$_cpofrm->addSubTag(new tag(new tipotag("IMG",false),
													Array(new atributo("SRC","framework/imagens/proximo.png"),
														  new atributo("ALIGN","ABSMIDDLE"),
														  new atributo("BORDER","0"),
														  new atributo("STYLE","cursor:pointer;"),
														  new atributo("ONCLICK",
														  "ObjProcAjax.runPost('executa_busca_ajax.php5','{$this->_nome}',
														  'classe={$this->_classe}&metodo=getProximoCodigo&arquivo_classe={$this->_arquivo_classe}');"))));
						 }
		}
		if($this->_valor!==null&&in_array(strtolower($this->_comportamento_form),Array('password','file',null))) {
			$_cpofrm->addAtributo(new atributo('VALUE',$this->toHTML()));
		}
		return $_cpofrm;
	}
}

/**
 * Classe para gerenciamento de campos do tipo string
 *
 */
class string extends campo {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false,bancodados &$_conn=null) {

		parent::__construct($_nome,$_titulo,'STR',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk,$_conn);
	}
	
	public function toBD() {
		return "'" . $this->_conn->EscapeString($this->_valor) . "'";
	}
	
	public function toHTML() {
		return $this->_valor;
	}
	
}

/**
 * Classe para gerenciamento de campos num�ricos
 *
 */
class numero extends campo {
	protected $_casas_decimais = 99;
	
	public function __construct($_nome,$_titulo,$_tipo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false) {

		parent::__construct($_nome,$_titulo,$_tipo,$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk);
	}
	
	public function toBD() {
		return number_format(str_replace(",",".",str_replace(".","",$this->_valor)),$this->_casas_decimais,".","");
	}
	
	public function toHTML() {
		return number_format($this->_valor,$this->_casas_decimais,",","");
	}
	
	public function getAtributosExtras() {
		return new atributo("ALIGN","RIGHT");
	}
}

/**
 * Classe derivada da classe numero para gerenciar inteiros
 *
 */
class inteiro extends numero {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false) {

		parent::__construct($_nome,$_titulo,'INT',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk);
		$this->_casas_decimais = 0;
	}
}


/**
 * Classe derivada da classe numero para gerenciar n�mero de ponto flutuante
 *
 */
class float extends numero {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false,$_casas=3) {

		parent::__construct($_nome,$_titulo,'FLT',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk);
		$this->_casas_decimais = $_casas;
	}
}

/**
 * Classe derivada da classe numero para gerenciar valores de Dinheiro
 *
 */
class dinheiro extends numero {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false) {

		parent::__construct($_nome,$_titulo,'CUR',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk);
		$this->_casas_decimais = 2;
	}
}


/**
 * Classe para gerenciar campos do tipo data (sem informa��o de hora)
 *
 */
class data extends campo {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false,bancodados &$_conn=null) {

		parent::__construct($_nome,$_titulo,'DATE',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk,$_conn);
	}
	
	public function toBD() {
		return $this->_conn->converteDataToBD($this->_valor);
	}
	
	public function toHTML() {
		return $this->_conn->converteDataToHTML($this->_valor);
	}

	public function toForm() {
		$_cpofrm = new formInputTexto($this->_nome,'DATE',12,10);
		// Bot�o adicional
		$_cpofrm->addSubTag(new tag(new tipotag('IMG',false),Array(
											new atributo('SRC','framework/imagens/calendar.png'),
											new atributo('ALIGN','ABSBOTTOM'),
											new atributo('STYLE','cursor:pointer;'),
											new atributo('ONCLICK',
				"var cal=new Epoch('cal','popup',document.getElementById('{$this->_nome}'),false,true);"))));
		if($this->_valor!==null) {
			$_cpofrm->addAtributo(new atributo('VALUE',$this->toHTML()));
		}
		$_cpofrm->setVerificaCampo('DATE');
		$_cpofrm->setValidaCampo('DATE',$this->_nome,$this->_titulo,$this->_minimo,$this->_maximo,$this->_obrigatorio);
		return $_cpofrm;
	}
	
}

/**
 * Classe para gerenciar campos do tipo data e hora
 *
 */
class datahora extends campo {
	public function __construct($_nome,$_titulo,$_tamanho,$_formato,$_relacao,
								$_incluir=true,$_alterar=true,$_listar=true,$_minimo=null,
								$_maximo=null,$_camporef=null,$_pk=false,bancodados &$_conn=null) {

		parent::__construct($_nome,$_titulo,'DTIME',$_tamanho,$_formato,$_relacao,
								$_incluir,$_alterar,$_listar,$_minimo,
								$_maximo,$_camporef,$_pk,$_conn);
	}
	
	public function toBD() {
		return $this->_conn->converteDataHoraToBD($this->_valor);
	}
	
	public function toHTML() {
		return $this->_conn->converteDataHoraToHTML($this->_valor);
	}
	
	public function toForm() {
		$_cpofrm = new formInputTexto($this->_nome,'DTIME',16,19);
		// Bot�o adicional
		$_cpofrm->addSubTag(new tag(new tipotag('IMG',false),Array(
											new atributo('SRC','framework/imagens/calendar.png'),
											new atributo('ALIGN','ABSBOTTOM'),
											new atributo('STYLE','cursor:pointer;'),
											new atributo('ONCLICK',
				"var cal=new Epoch('cal','popup',document.getElementById('{$this->_nome}'),false,true);"))));
		if($this->_valor!==null) {
			$_cpofrm->addAtributo(new atributo('VALUE',$this->toHTML()));
		}
		$_cpofrm->setVerificaCampo('DTIME');
		$_cpofrm->setValidaCampo('DTIME',$this->_nome,$this->_titulo,$this->_minimo,$this->_maximo,$this->_obrigatorio);
		return $_cpofrm;
	}
}

?>