<?php
require_once("conexion.php");
?>
<!DOCTYPE HTML>
<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <script type="text/javascript">
            setTimeout('document.location.reload()',5000)//especificamos los milisegundos en que la página se recarga
        </script>
        <script type="text/javascript" src="jquery.js"></script>
        <style type="text/css">
        ${demo.css}
        </style>
        <script type="text/javascript">
    $(function () {
        $('#container').highcharts({
            title: {
                text: 'Reporte Numero de personas',
                x: -20 //center
            },
            subtitle: {
                text: '',
                x: -20
            },
            xAxis: {
                title: {
                    text: 'hora'
                },
                categories: [
                <?php
                    $sql = " select HOUR(fecha_hora) as fh,MINUTE(fecha_hora) as fm from registro GROUP BY HOUR(fecha_hora)";
                    $result = mysqli_query($connection, $sql);
                    while($registros = mysqli_fetch_array($result)){
                        ?>
                            '<?php echo  $registros["fh"]?>',
                        <?php
                    }
                ?>
                ]
            },
            yAxis: {
                title: {
                    text: 'Contador'
                },
                plotLines: [{
                    value: 0,
                    width: 1,
                    color: '#808080'
                }]
            },
            tooltip: {
                valueSuffix: ' Personas'
            },
            legend: {
                layout: 'vertical',
                align: 'right',
                verticalAlign: 'middle',
                borderWidth: 0
            },
            series: [
            {   name: 'Numero de Personas',
                data: [
                <?php
                    $query = "select count(*) as co, fecha_hora from registro group by HOUR(fecha_hora)";
                    $resultados = mysqli_query($connection, $query);
                    
                    while($rows = mysqli_fetch_array($resultados)){
                        ?>
                            <?php echo $rows["co"]?>,
                        <?php
                    }
                    
                ?>
                ]
            }
            ]
        });
});
        </script>
    </head>
    <body>
<script src="highcharts.js"></script>
<script src="exporting.js"></script>

<div id="container" style="min-width: 310px; height: 400px; margin: 0 auto"></div>

    </body>
</html>