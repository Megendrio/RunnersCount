<?php
/**
 * Created by Stefan De Raedemaeker
 * Date: 14/09/2016
 * Name: RunnerShow.php
 * Description: Show a screen with information about the runners to supporters
 */
include('misc.inc');
$version="v0.1";
$URL='RunnerShow.php';
$cxn=mysqli_connect($host,$user,$password,$dbname) or die("Couldn't connect to the database.");
header("Refresh:5");
?>
<head>
    <title>24 urenloop - Industria vzw</title>
    <link rel="stylesheet" type="text/css" href="css/main.php" />
    <style>
		.progressbar
		{
			position: relative;
			left:180px; top: -40px;
			width: 70%;
			min-width: 300px;
		}
		.record
		{
			width: 60%;
			float: left;
			margin-bottom: 0px; margin-top: 10px; margin-left: 20px;
			padding: 0px;
			min-width: 250px;
		}
		.circle
		{
			width: 50px;
			height: 50px;
			-webkit-border-radius: 25px;
			-moz-border-radius: 25px;
			border-radius: 25px;
			position: absolute;
			top: -25px;
			text-align: center;
			color: white;
			line-height: 50px;
			font-size: 30px;
			text-shadow: 1px 1px 1px #373737;
		}
		.circle_text
		{
			position: absolute;
			width: 140px;
			text-align: center;
			top: 30px;
			left: -45px;
			font-size: 1.2em;
			line-height: .9em;
		}
	</style>
</head>
<body>
<?php include_once 'includes/leftClock.php';?>

<div class="right">
    <h2>Statistieken</h2>

    <div class="stat_info" style="margin-top: 40px">
        <h3>Beste lopers</h3>
        <p>meest aantal toeren</p>
    </div>

    <div class="progressbar">
        <?php most_rounds($cxn) ?>
        <hr style="margin-top: 40px; color: #24499f; z-index: -1;">
    </div>

    <div class="stat_info">
        <h3>Snelste lopers</h3>
        <p>gem. snelste toeren</p>
    </div>

    <div class="progressbar">
        <?php fastest_runner($cxn)?>
        <hr style="margin-top: 40px; color: #24499f; z-index: -1;">
    </div>
</div>


<div class="record">
    <h3>Record snelste ronde</h3>
    <div style="font-size: 1.1em"><?php fastest_round($cxn) ?></div>
</div>
</body>
</html>
