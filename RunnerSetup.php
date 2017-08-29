<?php
/**
 * Created by Stefan De Raedemaeker
 * Date: 14/09/2016
 * Name: RunnerSetup.php
 * Description: - Scan student number
 *              - Search student in database
 *              - Show student information in form
 *              - Update student information
 *              - Add user to database
 */
include('misc.inc');
$version="v0.1";
$URL='RunnerSetup.php';
$cxn=mysqli_connect($host,$user,$password,$dbname) or die("Couldn't connect to the database.");
session_start();
if (isset($_SESSION['refresh_delay'])&&$_SESSION['refresh_url']=="RunnerSetup.php") //delay with http-equiv
{
    ?>
    <meta http-equiv="refresh" content="<?php echo $_SESSION['refresh_delay']?>;URL='RunnerSetup.php'">
    <?php
}
?>
<head>
    <title>Setup 24 urenloop | Industria vzw</title>
    <link rel="stylesheet" type="text/css" href="css/main.php" />
    <style>
        .left_container, .right_container
        {
			position: relative;
			top: 20px;
            float: left;
            margin: 10px 0px 10px 20px;
            padding: 10px;
            min-width: 500px;
            width: 35%;
            border-width: 1px;
            border-radius: 8px;
            border-style: solid;
        }
        label.field
        {
            text-align: right;
            width: 100px;
            float: left;
            font-weight: bold;
            margin-right: 4px;
        }

    </style>
</head>
<body>
<?php include_once 'includes/leftMenu.php';?>

<div class="left_container">
<h2>Runner Setup</h2>

<form action="RunnerSetup.php" method="post">
	<label class="field" for="studentID">Student ID:</label><input type="text" name="studentID" id="studentID" autofocus="true" size="8" maxlength="8">
	<input type="submit" value="Search student">
</form>

<?php
/* Clean up the user input for StudentID */
if(isset($_POST['studentID']))
{
    $studentID=$_POST['studentID'];
    trim($studentID);
    stripslashes($studentID);
    htmlspecialchars($studentID);
    $pattern='/^[a-z]{1}[0-9]{7}/';
    if(preg_match($pattern,$studentID)) {   //check pattern

        $_SESSION['studentID']=$studentID;
        ############################################
        ##   Form to add and update runner data   ##
        ############################################

        $query = "SELECT * FROM runner LEFT JOIN team ON (runner.team_id = team.team_id) WHERE runner.student_id='{$studentID}'";   //fetch all runner data
        $team_query="SELECT team_name FROM runner LEFT JOIN team  ON runner.team_id=team.team_id WHERE student_id ='{$studentID}'"; //fetch team name of runner
        $all_teams_query="SELECT * FROM team ORDER BY team.team_name";  //fetch all team names
        $result = mysqli_query($cxn, $query) or die(mysqli_error($cxn));
        $result2=mysqli_query($cxn,$team_query);
        $result3=mysqli_query($cxn,$all_teams_query);
        $runner_data = mysqli_fetch_assoc($result);   //store array with runner data
        $teamName=mysqli_fetch_assoc($result2);      //store team name

        if (!empty($runner_data))   // Check if runner is found
        {
            echo "<p class='message'>The runner with student ID <b><i>".$studentID."</i></b> is found in our system!<br>
                     Please double check if all given information below is correct.</p>\n";
            $new_runner=0;
        } else {
            echo "<p class='message'>We didn't find the runner with student ID <b><i>".$studentID."</i></b> in our system. <br>
                     Please fill in <u>ALL fields</u> below to ensure the runner can be queued.</p>\n";
            $new_runner=1;
        }

        echo "<form action='$URL' method='POST'>\n";
        echo "<p style='margin: 5px'><label class='field' for='fname'>First name:</label><input type='text' name='fname' id='fname' size='25' maxlength='25' value='{$runner_data['fname']}'></p>\n";
        echo "<p style='margin: 5px'><label class='field' for='lname'>Last Name:</label><input type='text' name='lname' id='lname' size='25' maxlength='25' value='{$runner_data['lname']}'></p>\n";
        echo "<p style='margin: 5px'><label class='field' for='student_id'>Student ID:</label><input type='text' name='student_id' id='student_id' size='8' maxlength='8' value='$studentID' disabled='true'></p>\n";
        echo "<p style='margin: 5px'><label class='field' for='points'>Points:</label><input type='number' name='points' id='points' size='8' maxlength='8' value='{$runner_data['points']}'></p>\n";
        echo "<p style='margin: 5px'><label class='field' for='team_name'>Team name:</label><select name='team_name' id='team_name'>";
                while($row=mysqli_fetch_assoc($result3))    //Dynamically change the options
                {
                    extract($row);
                    echo"<option value='$team_name'\n";
                    if($teamName['team_name']==$team_name)echo " selected='true'";
                    echo " >$team_name\n";
                }
        echo "</select></p>\n";


        echo "<div style='width: 300px; margin: auto'><input type='submit' name='submit' id='submit' value='Save'>\n";    //submit button
        echo "<input type='submit' name='add_queue' id='add_queue' value='Save & Add to queue'></div>\n";

        echo "<input type='hidden' name='update' value='update'>";    //hidden field to trigger save/update functions
        echo "<input type='hidden' name='runnerID' value='$studentID'>";
        echo "<input type='hidden' name='new_runner' value='$new_runner'>";
        echo"</form>\n";

        echo "<form action='RunnerQueue.php' method='post'>
                <input type='hidden' name='student_id' id='student_id' value='$studentID'></form>";   // send studentID to RunnerQueue.php

    }
    else echo"<p class='error'>The syntax of the Student ID is incorrect.<br>Please rescan the student card to try again.</p>";
}
else echo"<p>Scan a student card or fill in a student number.</p>";

/* Insert or update data in database when form is submitted */
if(isset($_POST['update'])&&$_POST['runnerID'])
{
    $studentID=$_POST['runnerID'];
    $fname = $_POST['fname'];
    $lname = $_POST['lname'];
    $points = $_POST['points'];
    $new_runner=$_POST['new_runner'];
    $team_name=$_POST['team_name'];
    //echo "<p class='success'>Successfully submitted!</p>";

    $match_query="SELECT team_id FROM team WHERE team_name=\"{$team_name}\"";
    $result4=mysqli_query($cxn,$match_query);
    $id_match=mysqli_fetch_assoc($result4);
    $team_id=$id_match['team_id'];

    /* update data of existing runner */
    if ($new_runner == 0)
    {
        update_data($cxn, $studentID, $fname, $lname, $points, $team_id);
    }

    /* insert data of new runner */
    if ($new_runner == 1)
    {
        insert_data($cxn, $studentID, $fname, $lname, $points, $team_id);
    }
}
?>
</div>
<div class="right_container">
<h2>Runner Queue</h2>
<?php
if(isset($_POST['runnerID'])&&isset($_POST['add_queue']))
{
    $studentID=$_POST['runnerID'];
    echo "<p class='message'>Average: ".good_runner($cxn,$studentID)." seconds</p>";
}
echo "<hr>";
show_queued($cxn,$URL);
?>
</div>
</body>
</html>
