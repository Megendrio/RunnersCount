<?php
$user="root";
$host="localhost";
$password="fun";
$dbname="24hour_run";

function refresh($URL,$delay) //refresh the page by submitting form
{
    $_SESSION['refresh_url']=$URL;
    $_SESSION['refresh_delay']=$delay;
}

function minsec($sec)
{
    $min = floor($sec/60);
    $sec= ($sec%60);
    echo "{$min}' {$sec}\"";
}

function insert_data($cxn,$studentID,$fname,$lname,$points,$team_id)    //Check entered data for vulnerabilities and insert data from new runner in database
{
    //Check entered data for vulnerabilities
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $fname = test_input($fname);
        $lname = test_input($lname);
        $points = test_input($points);
        $team_id = test_input($team_id);
    }

    $query3="INSERT INTO runner (student_id, fname, lname, points,team_id) VALUES (\"{$studentID}\",\"{$fname}\",\"{$lname}\",\"{$points}\",\"{$team_id}\")";
    $insert = mysqli_query($cxn, $query3) or die("<p class='error'>The new runner can not be saved in the system.<br>Check if you entered all required fields!</p>");
    if($insert) echo "<p class='success'>Successfully saved!</p>";  //when update is successful -> true

}

function update_data($cxn,$studentID,$fname,$lname,$points,$team_id)    //Check entered data for vulnerabilities and update runner data in database
{
    //Check entered data for vulnerabilities
    if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        $fname = test_input($fname);
        $lname = test_input($lname);
        $points = test_input($points);
    }

    //Update the entered runner data from the form in the database
    $query2 = "UPDATE runner SET fname=\"{$fname}\", lname=\"{$lname}\", points=\"{$points}\", team_id=\"{$team_id}\" WHERE runner.student_id=\"{$studentID}\"";
    $update=mysqli_query($cxn,$query2) or die("<p class='error'>The runner data can not be updated.<br>".mysqli_error($cxn)."</p>");
    if($update) echo "<p class='success'>Successfully updated!</p>";    //when update is successful -> true
}

function test_input($data)  //Remove vulnerabilities from entered data
{
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}

function runner_average($cxn,$studentID)
{
    $times=array(); //initialize array to store times
    $query="SELECT * FROM round WHERE student_id=\"{$studentID}\" LIMIT 5";
    $result=mysqli_query($cxn,$query) or die("Could't execute the query.");

    $loop=0;
    while($row=mysqli_fetch_assoc($result))
    {
        extract($row);
        $start=$s_time;     //extract row with times from query
        $end=$e_time;
        $time= strtotime($end) - strtotime($start); //time = end-start
        $times[$loop]=$time;    //store time in array with index number=#loop
        $loop++;
    }
    $count=count($times);   //get number of times stored in time array
    if(isset($count)&&$count!=0)    //when divisible
    {
        $avg = array_sum($times) / count($times);   //calculate average time for runner
    }
    else $avg=0;
    return $avg;
}

function good_runner($cxn,$studentID)   //test if a runner is fast enough for a flagged "speed" period
{
    $avg=runner_average($cxn,$studentID);

    $flag_array=get_flag($cxn);
    if($flag_array['flag']==1)
    {
        if($avg<=97)
        {
            echo "<p class='success'>{$_POST['fname']} {$_POST['lname']} is added to the queue.</p>";
            add_queue($cxn,$studentID);
        }
        else
        {
            echo "<p class='error'>{$_POST['fname']} {$_POST['lname']} cannot be added to the queue for this period.</p>";  //error message
        }
    }
    else
    {
        echo "<p class='success'>{$_POST['fname']} {$_POST['lname']} is added to the queue.</p>";
        add_queue($cxn,$studentID);
    }
    return $avg;
}

function add_queue($cxn,$studentID) // add a runner to queue
{
    $query="INSERT INTO queue (student_id) VALUES (\"{$studentID}\")";
    mysqli_query($cxn,$query);
    refresh("all",1);

}

function get_flag($cxn) //retrieve state of flag for speed period from database
{
    $get_flag_query="SELECT * FROM period WHERE period_id=(SELECT max(period_id) FROM period)";   //fetch data from period
    $flag_execute=mysqli_query($cxn,$get_flag_query) or die("<p class='error'>Could't execute get_flag query!</p>");
    return mysqli_fetch_assoc($flag_execute);
}

function update_flag($cxn,$URL) //update state of flag for speed period in database
{
    if(isset($_POST['prev_flag']))  //get previous state of flag with _POST
    {
        $flag="";   //initialize flag
        $prev_flag = $_POST['prev_flag'];
        if (isset($_POST['set_flag']))  //get current state of flag with _POST
        {
            $flag = $_POST['set_flag'];
        }

        if ($flag != $prev_flag)
        {
            if ($flag == 1) //speed period is set
            {
                $timestamp = date("Y-m-d H:i:s",strtotime('+2 hours'));
                $update_query1 = "UPDATE period SET e_period=\"{$timestamp}\" ORDER BY period_id DESC LIMIT 1"; //update end time from previous period
                mysqli_query($cxn, $update_query1) or die("<p class='error'>Could't execute update query 1</p>");

                $set_query1 = "INSERT INTO period (flag,s_period) VALUES (1,\"{$timestamp}\")"; //insert new flag with start time
                mysqli_query($cxn, $set_query1) or die("<p class='error'>Could't execute set query 1</p>");
                echo "<p class='message'>The speed period is started.</p>";
            }
            else    //speed period is unset
            {
                $timestamp = date("Y-m-d H:i:s",strtotime('+2 hours'));
                $update_query1 = "UPDATE period SET e_period=\"{$timestamp}\" ORDER BY period_id DESC LIMIT 1"; //update end time from previous period
                mysqli_query($cxn, $update_query1) or die("<p class='error'>Could't execute update query 1</p>");

                $set_query1 = "INSERT INTO period (flag,s_period) VALUES (0,\"{$timestamp}\")"; //insert new flag with start time
                mysqli_query($cxn, $set_query1) or die("<p class='error'>Could't execute set query 1</p>");
                echo "<p class='message'>The speed period is stopped.</p>";
            }
            refresh($URL,2);
        }
    }
}

function show_queued($cxn,$URL) //show a list of all queued runners
{
    $select_query="SELECT runner.fname, runner.lname, runner.student_id, queue.q_id, team.team_name FROM runner RIGHT JOIN queue ON (runner.student_id = queue.student_id) LEFT JOIN team ON (runner.team_id=team.team_id) ORDER  BY queue.q_id ASC";
    $execute_query=mysqli_query($cxn,$select_query)or die("<p>Could't execute show queued query</p>");

    echo "<table width='450px' style='text-align: left'>";
    echo "<tr><th>Name</th><th>Team</th><th>Average</th><th>Actions</th></tr>"; //titles of the table
    $loop0=0;
    while($data_row=mysqli_fetch_assoc($execute_query))
    {
        extract($data_row);
        if($loop0==0&&$URL=='RunnerQueue.php'){}    //don't show first runner in queue when on RunnerQueue.php page
        else
        {
            $times_array=array(); //initialize array to store times
            $times_query="SELECT * FROM round WHERE student_id=\"{$student_id}\"";
            $result=mysqli_query($cxn,$times_query) or die("Could't execute the query.");

            $loop1=0;
            while($time_row=mysqli_fetch_assoc($result))
            {
                extract($time_row);
                $start=$s_time;     //extract row with times from query
                $end=$e_time;
                $time= strtotime($end) - strtotime($start); //time = end-start
                $times_array[$loop1]=$time;    //store time in array with index number=#loop
                $loop1++;
            }
            $count_times=count($times_array);   //get number of times stored in time array
            if(isset($count_times)&&$count_times!=0)    //when divisible
            {
                $avg = array_sum($times_array) / count($loop1);   //calculate average time for runner
            }
            else $avg=0;

            echo "<tr>
                <td>{$fname} {$lname}</td>    
                <td>{$team_name}</td>
                <td>{$avg} sec.</td>
                <td><form action='{$URL}' method='post'>
                <input type='submit' name='move_first' id='move_first' value='&#x2776' style='color: #24499f' title='Move runner to first place'> <!-- button to move runner to first place in queue-->
                <input type='submit' name='delete' id='delete' value='&#x2718'  style='color: red' title='Delete runner from queue'> <!-- button to delete runner from queue -->
                <input type='hidden' name='queue_id' id='queue_id' value='{$q_id}'>
                </form>
                </td>
              </tr>";
        }
        $loop0++;
    }

    //decision based on the selected actions on button click
    if(isset($_POST['queue_id']))
    {
        $queue_id=$_POST['queue_id'];
        if(isset($_POST['delete']))
        {
            queue_action($cxn,$queue_id,"delete",$URL);
        }
        else if(isset($_POST['move_first']))
        {
            queue_action($cxn,$queue_id,"move_first",$URL);
        }
    }
    echo "</table>";
}

function show_queued_simple($cxn,$URL) //show a list of all queued runners withouth extra options
{
    $select_query="SELECT runner.fname, runner.lname, runner.student_id, queue.q_id, team.team_name FROM runner RIGHT JOIN queue ON (runner.student_id = queue.student_id) LEFT JOIN team ON (runner.team_id=team.team_id) ORDER  BY queue.q_id ASC";
    $execute_query=mysqli_query($cxn,$select_query)or die("<p>Could't execute show queued query</p>");

    echo "<table width='400px' style='text-align: left'>";
    echo "<tr><th>Name</th><th>Team</th><th>Average</th></tr>"; //titles of the table
    $loop0=0;
    while($data_row=mysqli_fetch_assoc($execute_query))
    {
        extract($data_row);
		$times_array=array(); //initialize array to store times
		$times_query="SELECT * FROM round WHERE student_id=\"{$student_id}\"";
		$result=mysqli_query($cxn,$times_query) or die("Could't execute the query.");

		$loop1=0;
		while($time_row=mysqli_fetch_assoc($result))
		{
			extract($time_row);
			$start=$s_time;     //extract row with times from query
			$end=$e_time;
			$time= strtotime($end) - strtotime($start); //time = end-start
			$times_array[$loop1]=$time;    //store time in array with index number=#loop
			$loop1++;
		}
		$count_times=count($times_array);   //get number of times stored in time array
		if(isset($count_times)&&$count_times!=0)    //when divisible
		{
			$avg = array_sum($times_array) / count($loop1);   //calculate average time for runner
		}
		else $avg=0;

		echo "<tr>
			<td>{$fname} {$lname}</td>    
			<td>{$team_name}</td>
			<td>{$avg} sec.</td>
		  </tr>";
        $loop0++;
    }

    //decision based on the selected actions on button click
    if(isset($_POST['queue_id']))
    {
        $queue_id=$_POST['queue_id'];
        if(isset($_POST['delete']))
        {
            queue_action($cxn,$queue_id,"delete",$URL);
        }
        else if(isset($_POST['move_first']))
        {
            queue_action($cxn,$queue_id,"move_first",$URL);
        }
    }
    echo "</table>";
}

function queue_action($cxn,$queue_id,$action,$URL)  //execute the 'move_up' or 'delete' actions
{
    if($action=="delete")   //delete the selected runner from queue
    {
        $delete_query="DELETE FROM queue WHERE q_id=\"{$queue_id}\"";
        mysqli_query($cxn,$delete_query) or die("<p class='error'>Could't delete</p>".mysqli_error($cxn));
        echo "<p class='success'>Successfully deleted from queue!</p>";
    }
    else if($action=="move_first")  //move runner to first place in queue
    {
        $last_qid_query="SELECT MAX(q_id) FROM queue";  //get the queue_id from the last runner in queue
        $last_qid_execute=mysqli_query($cxn,$last_qid_query) or die("<p class='error'>".mysqli_error($cxn)."</p>");
        $last_qid_array=mysqli_fetch_row($last_qid_execute);
        $last_qid=$last_qid_array[0]+1; //new queue_id after last one

        $mvto_lastqid_query="UPDATE queue SET q_id=\"{$last_qid}\" WHERE q_id=\"{$queue_id}\""; //move selected runner to new queue position
        mysqli_query($cxn,$mvto_lastqid_query) or die("<p class='error'>".mysqli_error($cxn)."</p>");

        $get_queue_query="SELECT * FROM queue WHERE q_id != \"{$last_qid}\" ORDER BY q_id ASC"; //get all runners in queue ascending except the selected one
        $get_queue_execute=mysqli_query($cxn,$get_queue_query) or die("<p class='error'>".mysqli_error($cxn)."</p>");

        $i=1;
        while($row=mysqli_fetch_assoc($get_queue_execute))  //move every runner one by one to new queue_id (one further than previous id)
        {
            extract($row);
            $new_qid=$last_qid+$i;
            $mv_qid_query="UPDATE queue SET q_id=\"{$new_qid}\" WHERE q_id=\"{$q_id}\"";
            mysqli_query($cxn,$mv_qid_query) or die("<p class='error'>".mysqli_error($cxn)."</p>");
            $i++;
        }
        //set new increment start value after last queue_id
        $incr_strt=$last_qid+$i;
        $autoincr_corr_query="ALTER TABLE queue AUTO_INCREMENT={$incr_strt}";
        mysqli_query($cxn,$autoincr_corr_query)  or die("<p class='error'>alter table ".mysqli_error($cxn)."</p>");
        echo "<p class='success'>Moved to first place in queue!</p>";
    }
    refresh("all",1);
}

function runner_now($cxn,$URL) //show the first runner in queue with GO!/STOP! buttons
{
    $query="SELECT runner.fname, runner.lname,runner.student_id,queue.q_id,team.team_name FROM runner RIGHT JOIN queue ON (runner.student_id = queue.student_id) LEFT JOIN team ON (runner.team_id=team.team_id) ORDER  BY queue.q_id ASC LIMIT 1";
    $execute_query=mysqli_query($cxn,$query)or die("<p>Could't execute show queued query</p>"); //get first runner in queue
    while ($row=mysqli_fetch_assoc($execute_query))
    {
        extract($row);
        echo "<form action='$URL' method='post'>";// hier verder zoeken-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
        echo "<label class='runner_now' for='start_round'>{$fname} {$lname}</label>";   //show first name and last name

		/*
		 * when runner is the very first one to run a round
		 * a normal runner in queue, with other runners queued after this runner
		 * when no other runners are queued after this runner
		 * */
        if (queue_status($cxn) == "first" || queue_status($cxn) == "default" || queue_status($cxn) == "last")  
        {
            echo "<input type='submit' name='start_round' id='start_round' value='GO!' autofocus='autofocus' style='width: 60px; height: 40px; background-color: green;color: white' >";
        }
        echo "<input type='hidden' name='curr_qid' id='curr_qid' value='{$q_id}'>"; //send queue_id with _POST
        echo "<input type='hidden' name='stud_id' id='stud_id' value='{$student_id}'></form>";  //send student_id with _POST
    }
    if (queue_status($cxn) == "empty")  //when current runner is running and no other runner is queued
    {
        echo "<form action='$URL' method='post'>";
        echo "<input type='submit' name='stop_round' id='stop_round' value='STOP!' autofocus='autofocus' style='width: 60px; height: 40px; background-color: red;color: white'>";
    }
}

function queue_status($cxn) //gives the status of the queue e.g.: first, empty, last, default
{
    //Count runners left in queue by fetching all runners from queue and extracting in while loop
    $query="SELECT runner.fname, runner.lname,runner.student_id,queue.q_id,team.team_name FROM runner RIGHT JOIN queue ON (runner.student_id = queue.student_id) LEFT JOIN team ON (runner.team_id=team.team_id) ORDER  BY queue.q_id ASC";
    $execute_query=mysqli_query($cxn,$query)or die("<p class='error'>Could't execute show queued query</p>");
    $count=0;
    while ($row=mysqli_fetch_assoc($execute_query))
    {
        extract($row);
        $count ++;
    }

    //look if first round is already set.
    $query2="SELECT round_id FROM round ORDER BY round_id ASC";
    $execute_query2=mysqli_query($cxn,$query2) or die(mysqli_error($cx));
    $rounds_array=mysqli_fetch_assoc($execute_query2);
    $rounds=count($rounds_array);   //count rounds

    //return value depending on queue status
    if($rounds==0) return "first";
    else if($count==0) return "empty";
    else if($count==1) return "last";
    else return "default";
}

function runner_finish($cxn,$current_time,$studentID,$queue_id,$URL)   //actions when GO! button is pressed
{
    if(queue_status($cxn)=="default")
    {
        $update_prev_round_query="UPDATE round SET e_time=\"{$current_time}\" ORDER BY round_id DESC LIMIT 1";  //update end time in previous round
        mysqli_query($cxn,$update_prev_round_query) or die(mysqli_error($cxn));
    }
    $student_id_query="SELECT student_id,round_id FROM round ORDER BY round_id DESC LIMIT 1";
    $get_student_id=mysqli_query($cxn,$student_id_query) or die($cxn);
    $prev_student_id="";
    while($row=mysqli_fetch_assoc($get_student_id))
    {
        extract($row);
        $prev_student_id=$student_id;
    }

    $query="INSERT INTO round (student_id,s_time) VALUES (\"{$studentID}\",\"{$current_time}\")";  //insert new round
    mysqli_query($cxn,$query) or die(mysqli_error($cxn));

    queue_action($cxn,$queue_id,"delete",$URL); //delete entry current runner from queue
    refresh($URL,0);
    runner_points($cxn,$prev_student_id);
}

function round_stop($cxn,$current_time,$URL)    //executed when STOP! button is pressed
{
    $update_round_query="UPDATE round SET e_time=\"{$current_time}\" ORDER BY round_id DESC LIMIT 1";   //update end time from last runner
    mysqli_query($cxn,$update_round_query) or die(mysqli_error($cxn));

    $student_id_query="SELECT student_id,round_id FROM round ORDER BY round_id DESC LIMIT 1";
    $get_student_id=mysqli_query($cxn,$student_id_query) or die($cxn);
    while($row=mysqli_fetch_assoc($get_student_id))
    {
        extract($row);
        $this_student_id=$student_id;
    }
    runner_points($cxn,$this_student_id);

    echo "<p class='success'>The runner data is updated!<br>Do NOT press the STOP! button again!</p>";
}

function runner_points($cxn,$student_id)    //calculate points based on the performance of the runner
{
    $get_points_query="SELECT points, student_id from runner WHERE student_id=\"{$student_id}\"";
    $execute_get_points=mysqli_query($cxn,$get_points_query) or die(mysqli_error($cxn));
    $curr_points=0;
    while($points_row=mysqli_fetch_assoc($execute_get_points))
    {
        extract($points_row);
        $curr_points=$points;
    }

    //calculate points based on time
    $flag=get_flag($cxn);
    $times_query="SELECT * FROM round WHERE student_id=\"{$student_id}\" ORDER BY round_id DESC LIMIT 1";
    $times_result=mysqli_query($cxn,$times_query) or die(mysqli_error($cxn));

    $new_points=0;
    while($times_row=mysqli_fetch_assoc($times_result)) //for each row from a runner
    {
        extract($times_row);
        $start=$s_time;     //extract row with times from query
        $end=$e_time;
        $time= strtotime($end) - strtotime($start); //time = end-start

        $get_special_period="SELECT * FROM special_period";
        $execute_special_period=mysqli_query($cxn,$get_special_period) or die(mysqli_error($cxn));
        $st_time="";
        $en_time="";
        while($period_row=mysqli_fetch_assoc($execute_special_period))
        {
            extract($period_row);
            $st_time=$start_time;
            $en_time=$end_time;
        }

        if(($st_time<=$start)&&$en_time>=$end)    //special period between start and end time
        {
            if($flag==1)    //flag set
            {
                if($time<90) $new_points=$new_points+3;
                else if($time<=97) $new_points=$new_points+2;
                else if($time<=120)  $new_points=$new_points+1;
            }
            else            //flag not set
            {
                if($time<90) $new_points=$new_points+3;
                else if($time<=120)  $new_points=$new_points+2;
                else if($time>120) $new_points=$new_points+1;
            }
        }
        else    //normal period
        {
            if($flag==1)    //flag set
            {
                if($time<90) $new_points=$new_points+2;
                else if($time<=97) $new_points=$new_points+1;
            }
            else            //flag not set
            {
                if($time<90) $new_points=$new_points+2;
                else if($time<=120)  $new_points=$new_points+1;
            }
        }
    }
    $set_points=$curr_points+$new_points;
    $update_points_query="UPDATE runner SET points=\"{$set_points}\" WHERE student_id=\"{$student_id}\"";
    mysqli_query($cxn,$update_points_query) or die($cxn);
}

function most_rounds($cxn)
{
    $most=null;
    $second=null;
    $third=null;
    $offset=0;
    $zindex=5;
    $most_rounds_q = "SELECT COUNT(round.round_id) AS rounds, runner.fname, runner.lname, runner.student_id FROM runner RIGHT join round ON (runner.student_id=round.student_id) GROUP BY student_id ORDER BY rounds DESC LIMIT 3";
    $most_rounds_x = mysqli_query($cxn, $most_rounds_q) or die(mysqli_error($cxn));
    for ($i = 0; $i < 3; $i++)
    {
        $rounds_row = mysqli_fetch_assoc($most_rounds_x);

        if(!is_null($rounds_row))
        {
            extract($rounds_row);
            $n=$i +1;
            if($n==1){$color="#E03F0F";$most=$rounds;}
            else if($n==2){$color="#9500B6";$second=$rounds;$offset=-115;}
            else if($n=3){$color="#24499f";$third=$rounds;}
            $zindex=$zindex-1;

            if($rounds==1)
            {
                $syntax="ronde";
            }
            else
            {
                $syntax="rondes";
            }

            $percentage=(($rounds/$most)*90);
            if ($second==$most && !is_null($second)){ $n=$n-1;}
            else if ($third==$second && !is_null($third)){ $n=$n-1; $offset=0;}
            else if($third!=$second && !is_null($third)){$offset=0;}
            echo "<div class='circle' style='background-color: {$color}; margin-left: {$percentage}%; z-index: $zindex'>{$n}</div>
             <div class='circle_text' style='margin-left: {$percentage}%; margin-top: {$offset}px; color: {$color}'>{$fname}<br><small>{$lname}</small><br><small>{$rounds} {$syntax}</small></div>";
        }
    }
}

function fastest_runner($cxn)
{
    $runner_avg = array();
    $all_runners_q = "SELECT DISTINCT runner.student_id FROM runner RIGHT join round ON (runner.student_id=round.student_id)  GROUP BY student_id ORDER BY student_id DESC";
    $all_runners_x = mysqli_query($cxn, $all_runners_q) or die(mysqli_error($cxn));
    while ($all_runner_row = mysqli_fetch_assoc($all_runners_x))
    {
        extract($all_runner_row);
        $runner_avg[$student_id] = runner_average($cxn, $student_id);
    }

    array_multisort($runner_avg, SORT_ASC);
    $runner_avg=array_slice($runner_avg,1,3);

    $first=null;
    $second=null;
    $third=null;
    $offset=0;
    $i = 0;
    $zindex=5;
    foreach ($runner_avg as $studentID => $average)
    {
        $student_q = "SELECT * FROM runner WHERE runner.student_id='{$studentID}'";
        $student_x = mysqli_query($cxn, $student_q) or die($cxn);
        $student = mysqli_fetch_assoc($student_x);
        $fname = $student['fname'];
        $lname = $student['lname'];
        $n = $i + 1;
        $zindex=$zindex-1;
        $avg=$average;
        if ($n == 1) {
            $color = "#E03F0F";
            $first = $avg;
        } else if ($n == 2) {
            $color = "#9500B6";
            $second = $avg;
            $offset = -115;
        } else if ($n = 3) {
            $color = "#24499f";
            $third = $avg;
        } else if ($n==4) break;

        $speed=((550/$avg)*3.6);

        $percentage = (($first / $avg) * 90)-(($i*20));
        if ($second == $first && !is_null($second)) {
            $n = $n - 1;
        } else if ($third == $second && !is_null($third)) {
            $n = $n - 1;
            $offset = 0;
        } else if ($third != $second && !is_null($third)) {
            $offset = 0;
        }

        $speed=number_format($speed,2);
        echo "<div class='circle' style='background-color: {$color}; margin-left: {$percentage}%; z-index: $zindex'>{$n}</div>
              <div class='circle_text' style='margin-left: {$percentage}%; margin-top: {$offset}px; color: {$color}'>{$fname}<br><small>{$lname}</small><br><small>{$speed}km/h</small></div>";
        $i++;
    }
}

function fastest_round($cxn)
{
    $fastest_runner_q="SELECT TIME_TO_SEC(TIMEDIFF(e_time,s_time)) AS time, runner.fname, runner.lname FROM runner RIGHT JOIN round ON (runner.student_id=round.student_id) ORDER BY time ASC LIMIT 2";
    $fastest_runner_x=mysqli_query($cxn,$fastest_runner_q) or die(mysqli_error($cxn));
    $already_set=0; //round with time 0s will also be fetched from query. -> set flag if speed record is already displayed
    for($i=0;$i<2;$i++)
    {
        $fastest_runner=mysqli_fetch_assoc($fastest_runner_x);
        $time=$fastest_runner['time'];
        if($time!=0 && !is_null($time) && $already_set==0)
        {
            $speed=(550/$time)*3.6;
            echo"{$fastest_runner['fname']} {$fastest_runner['lname']} liep een ronde in ";minsec($time);echo" met een snelheid van {$speed} km/h.";
            $already_set=1;
        }
    }
}

?>
