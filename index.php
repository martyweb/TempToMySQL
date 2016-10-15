<?php
//print_r($_REQUEST);

$mysqli = new mysqli("localhost", "adafruit", "adafruit", "adafruit");

/* check connection */
if ($mysqli->connect_errno) {
    printf("Connect failed: %s\n", $mysqli->connect_error);
    exit();
}

if(isset($_REQUEST['tmp'])){
   

    $query = "insert into `adafruit`.`new_table` (
        `temp`,`altitude`,`pressure`,`humidity`,`insertDate`,`insertTime`)
        values
        (".$_REQUEST['tmp'].",".$_REQUEST['alt'].",".$_REQUEST['pres'].",".$_REQUEST['hum'].",
        sysdate(), sysdate());";

    if ($mysqli->query($query) === TRUE) {
        
    }else{
        //print_r($query);
        printf("Error: %s\n", $mysqli->error);
    }

}else{


echo '

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <!-- The above 3 meta tags *must* come first in the head; any other head content must come *after* these tags -->
    <meta name="description" content="">
    <meta name="author" content="">
    <link rel="icon" href="http://v4-alpha.getbootstrap.com/dist/css/favicon.ico">

    <title>Starter Template for Bootstrap</title>

    <!-- Bootstrap core CSS -->
    <link href="http://v4-alpha.getbootstrap.com/dist/css/bootstrap.min.css" rel="stylesheet">

    <!-- Custom styles for this template -->
    <style>
        body {
        padding-top: 5rem;
        }
        .starter-template {
        padding: 3rem 1.5rem;
        text-align: center;
        }
    </style>
    
  </head>

  <body>

    <nav class="navbar navbar-fixed-top navbar-dark bg-inverse">
      <a class="navbar-brand" href="#">Adafruit Feather Temp Logger</a>
      <ul class="nav navbar-nav">
        <li class="nav-item">
          <a class="nav-link" href="https://www.adafruit.com/feather">Feather Info</a>
        </li>
        <li class="nav-item">
          <a class="nav-link" href="https://github.com/martyweb/tempToOLED">Source Code</a>
        </li>
      </ul>
    </nav>

    <div class="container">

    

';

    $query = "SELECT insertDate, hour(insertTime) time, 
        round(avg(altitude),2) altitude, round(avg(temp),2) temp,
        round(min(temp),2) mintemp, round(max(temp),2) maxtemp,
        round(avg(pressure),2) pressure, round(avg(humidity),2) humidity,
        count(*) records
        FROM `adafruit`.`new_table` 
        where insertDate >= sysdate() - interval 1 day 
        group by insertDate, hour(insertTime)
        order by concat(insertDate, ' ', insertTime) desc";

    $result = $mysqli->query($query);
    
    echo '<div class="table-responsive">
            <table class="table table-striped">
            <tr>
            <th>Date</th><th>Hour</th>
            <th>Temp</th><th>Min Temp</th><th>Max Temp</th>
            <th>Altitude</th>
            <th>Pressure</th><th>Humidity</th>
            <th>Records</th>
            </tr>';

    while ($row = $result->fetch_assoc()) {
        //print_r($row);
        echo "<tr><td>" . $row['insertDate'] . "</td><td>" . $row['time'] . "</td><td>" . $row['temp'] . "</td>
            <td>" . $row['mintemp'] . "</td><td>" . $row['maxtemp'] . "</td>
            <td>" . $row['altitude'] . "</td><td>" . $row['pressure'] . "</td><td>" . $row['humidity'] . "</td>
            <td>" . $row['records'] . "</td></tr>";
    }
    echo '</table>';

    $mysqli->close();

?>

    </div><!-- /.container -->
    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.0.0/jquery.min.js" integrity="sha384-THPy051/pYDQGanwU6poAc/hOdQxjnOEXzbT+OuUAFqNqFjL+4IGLBgCJC3ZOShY" crossorigin="anonymous"></script>
    <script>window.jQuery || document.write('<script src="../../assets/js/vendor/jquery.min.js"><\/script>')</script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/tether/1.2.0/js/tether.min.js" integrity="sha384-Plbmg8JY28KFelvJVai01l8WyZzrYWG825m+cZ0eDDS1f7d/js6ikvy1+X+guPIB" crossorigin="anonymous"></script>
    <script src="http://v4-alpha.getbootstrap.com/dist/js/bootstrap.min.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="http://v4-alpha.getbootstrap.com/assets/js/ie10-viewport-bug-workaround.js"></script>
  </body>
</html>


<?php

}
?>

