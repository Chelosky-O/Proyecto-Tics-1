<?php
class Herramienta{
	private $conexion;

	function __construct(){
		require_once("conexion_privada.php");
		$this->conexion = new conexion();
		$this->conexion->conectar();
	}

	public function ingresar_datos($huella, $temperatura){
		$sql = " insert into registro values (?, ?, CURRENT_TIME()) ";
		$stmt = $this->conexion->conexion->prepare($sql);

		$stmt->bindValue(1, $huella);
		$stmt->bindValue(2, $temperatura);

		if($stmt->execute()){
			echo "Ingreso Exitoso";
		}else{
			echo "no se pudo registrar datos";
		}
	}
}
?>