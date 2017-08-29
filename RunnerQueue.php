<?php
/**
 * Created by Stefan De Raedemaeker
 * Date: 14/10/2016
 * Name: RunnerQueue.php
 * Description: - ADD DESCRIPTION!!!
 */
include('misc.inc');
$version="v0.1";
$URL='RunnerQueue.php';
$cxn=mysqli_connect($host,$user,$password,$dbname) or die("Couldn't connect to the database.");
session_start();
if (isset($_SESSION['refresh_delay'])&&($_SESSION['refresh_url']=="RunnerQueue.php"||$_SESSION['refresh_url']=="all"))  //delay with http-equiv
{
    ?>
    <meta http-equiv="refresh" content="<?php echo $_SESSION['refresh_delay']?>;URL='RunnerQueue.php'">
    <?php
}
?>
<head>
    <title>Queue 24 urenloop | Industria vzw</title>
    <link rel="stylesheet" type="text/css" href="css/main.php" />
    <style>
        .current_runner,.queue
        {
            float: left;
            margin: 10px 5% 10px 5%;
            padding: 10px;
            min-width: 600px;
            width: 100%;
            border-width: 1px;
            border-radius: 8px;
            border-style: solid;
            text-align: center;
        }
        .runner_now
        {
            font-size: 1.7em;
            font-weight: bold;
            width:400px;
            color: #24499f;
            margin-right: 20px;
        }
    </style>
</head>
<body>
<?php include_once 'includes/leftMenu.php';?>

<div class="right">
	
<div class="current_runner">
    <?php
    echo"<h2>First in queue to run </h2>";
    echo runner_now($cxn,$URL);

    if(isset($_POST['start_round']))
    {
        $student_id=$_POST['stud_id'];
        $queue_id=$_POST['curr_qid'];
        $current_time=date("Y-m-d H:i:s",strtotime('+2 hours'));
        runner_finish($cxn,$current_time,$student_id,$queue_id,$URL);
    }
    else if(isset($_POST['stop_round']))
    {
        $current_time=date("Y-m-d H:i:s",strtotime('+2 hours'));
        round_stop($cxn,$current_time,$URL);
    }

    if(queue_status($cxn)=="first")
    {
        echo "<p class='message'>Welcome to the 24-Hour Run tracking system.<br>Scan a student card in the <a href='./RunnerSetup.php' target='_blank'>Setup 24 urenloop</a> page, and click 'Save & Add to queue'</p>";
        refresh($URL,5);
    }
    else if(queue_status($cxn)=="last")
    {
        echo "<p class='message'>This is the last runner in queue!<br>Scan a student card in the <a href='./RunnerSetup.php' target='_blank'>Setup 24 urenloop</a> page, and click 'Save & Add to queue'.</p>";
        refresh($URL,5);
    }
    else if(queue_status($cxn)=="empty")
    {
        echo "<p class='message'>The queue is empty! Click STOP when there isn't a next runner in time.<br>Scan a student card in the <a href='./RunnerSetup.php' target='_blank'>Setup 24 urenloop</a> page, and click 'Save & Add to queue'.</p>";
        refresh($URL,2);
    }
    else refresh($URL,5);
    ?>
</div>
<div class="queue">
    <?php
    show_queued($cxn,$URL);
    ?>
</div>

</div>


</body>
</html>
