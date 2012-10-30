<?php
/*
 * classe ProdutoGateway
 * implementa Row Data Gateway
 * 
 * Representa cada registro da tabela (cada linha)
 * N�o possue regras de neg�cio
 * StateFull
 */
class ProdutoGateway
{
    private $data;
    
    function __get($prop)
    {
        return $this->data[$prop];
    }
    
    function __set($prop, $value)
    {
        $this->data[$prop] = $value;
    }
    
    /*
     * m�todo insert
     * armazena o objeto na tabela de produtos
     */
    function insert()
    {
        // cria instru��o SQL de insert
        $sql = "INSERT INTO Produtos (id, descricao, estoque, preco_custo)" .
               " VALUES ('{$this->id}',      '{$this->descricao}', ".
               "         '{$this->estoque}', '{$this->preco_custo}')";
        echo $sql . "<br>\n";
        
        // instancia objeto PDO
        $conn = new PDO('sqlite:produtos.db');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        
        // executa instru��o SQL
        $conn->exec($sql);
        unset($conn);
    }
    
    /*
     * m�todo update
     * altera os dados do objeto na tabela de Produtos
     */
    function update()
    {
        // cria instru��o SQL de UPDATE
        $sql = "UPDATE produtos set ".
               "   descricao   = '{$this->descricao}', " .
               "   estoque     = '{$this->estoque}', ".
               "   preco_custo = '{$this->preco_custo}' ".
               "   WHERE id    = '{$this->id}'";
        echo $sql . "<br>\n";
        
        // instancia objeto PDO
        $conn = new PDO('sqlite:produtos.db');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        
        // executa a instru��o SQL
        $conn->exec($sql);
        unset($conn);
    }
    
    /*
     * m�todo delete
     * deleta o objeto da tabela de Produtos
     */
    function delete()
    {
        // cria instru��o SQL de DELETE
        $sql = "DELETE FROM produtos where id='{$this->id}'";
        echo $sql . "<br>\n";
        
        // instancia objeto PDO
        $conn = new PDO('sqlite:produtos.db');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        
        // executa instru��o SQL
        $conn->exec($sql);
        unset($conn);
    }
    
    /*
     * m�todo getObject
     * carrega um objeto a partir da tabela de produtos
     */
    function getObject($id)
    {
        // cria instru��o SQL de SELECT
        $sql = "SELECT * FROM produtos where id='{$id}'";
        echo $sql . "<br>\n";
        
        // instancia objeto PDO
        $conn = new PDO('sqlite:produtos.db');
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        
        // executa consulta SQL
        $result = $conn->query($sql);
        $this->data = $result->fetch(PDO::FETCH_ASSOC);
        unset($conn);
    }
}

// insere produtos na base de dados
$vinho= new ProdutoGateway;
$vinho->id          = 5;
$vinho->descricao   = 'Vinho Cabernet';
$vinho->estoque     = 10;
$vinho->preco_custo = 10;
$vinho->insert();

$salame= new ProdutoGateway;
$salame->id          = 6;
$salame->descricao   = 'Salame';
$salame->estoque     = 20;
$salame->preco_custo = 20;
$salame->insert();

// recupera um objeto e realiza altera��o
$objeto= new ProdutoGateway;
$objeto->getObject(6);
$objeto->estoque   = $objeto->estoque*2;
$objeto->descricao = 'Salaminho Italiano';
$objeto->update();

// exclui o produto vinho da tabela
$vinho->delete();
?>