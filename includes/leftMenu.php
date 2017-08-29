	<div id="header">
		<img src="images/industria_shield_logo.png" id="Industria_shield"><h1>24 urenloop - Industria</h1>
    </div>
	<div id="InfoAndSettings">
		<h4>Info & Settings</h4>
		<?php
		$cur_date=date("Y-m-d H:i:s",strtotime('+2 hours'));
		echo "<div style='font-size:16px;'>{$cur_date}</div>";  //display current time and date

		$flag_array=get_flag($cxn);

		echo"<form action='{$URL}' method='post' name='period_form' id='period_form'>\n";
		?>
		Speed period <input type='checkbox' name='set_flag' id='set_flag' value='1'
			<?php if($flag_array['flag']==1) echo "checked ='checked'";?> >

		<?php
		echo "<input type='hidden' name='prev_flag' id='prev_flag' value='{$flag_array['flag']}'>\n";
		echo "<input type='submit' name='submit_flag' id='submit_flag' value='Ok'></form>\n";

		if(isset($_POST['submit_flag']))
		{
			update_flag($cxn,$URL);
		}
		?>
	</div>
	
	<div id="left_background"></div>
<div class="left">
    <h2>Algemene info</h2>
    <table cellpadding="4px" style="margin-left: -4px">
        <tr><td width="150px"><b>rondes</b></td>
            <td>
                <?php
                $count_round_q="SELECT round_id FROM round";
                $count_round_x=mysqli_query($cxn,$count_round_q) or die(mysqli_error($cx));
                $total_rounds=0;   //count rounds                echo $total_rounds;
                while ($rounds_array=mysqli_fetch_assoc($count_round_x))
                {
                    $total_rounds++;
                }
                echo $total_rounds;
                ?>
            </td>
        </tr>
        <tr>
            <td><b>verstreken tijd</b></td>
            <td>
                <?php
                $get_time_query="SELECT s_time, round_id FROM round ORDER BY round_id ASC LIMIT 1";
                $get_time_exec=mysqli_query($cxn,$get_time_query) or die(mysqli_error($cxn));
                $start=date("Y-m-d H:i:s",strtotime('+2 hours'));
                while($row=mysqli_fetch_assoc($get_time_exec))
                {
                    extract($row);
                    $start=$s_time;
                }
                $passed_t= strtotime(date("Y-m-d H:i:s",strtotime('+2 hours'))) - strtotime($start);
                $hours = ($passed_t%86400)/3600;
                $minutes= (($passed_t%86400)%3600)/60;
                $hours=floor($hours);
                $minutes=floor($minutes);
                echo "{$hours}h {$minutes}min";
                ?>
            </td>
        </tr>
        <tr>
            <td><b>afstand</b></td>
            <td>
                <?php

                $meter=$total_rounds*550;
                $km=floor($meter/1000);
                $meter=floor($meter%1000);
                echo "{$km}km {$meter}m";
                ?>
            </td>
        </tr>
        <tr>
            <td><b>lopers</b></td>
            <td>
                <?php
                $count_runners_q="SELECT DISTINCT student_id FROM round";
                $count_runners_x=mysqli_query($cxn,$count_runners_q) or die(mysqli_error($cxn));
                $runner_count=0;
                while($count_array=mysqli_fetch_assoc($count_runners_x))
                {
                    $runner_count++;
                }
                echo $runner_count;
                ?>
            </td>
        </tr>
    </table>


    <h2 style="margin-top: 40px">Laatste lopers</h2>
    <table cellpadding="4px" style="margin-left: -4px">
        <tr><th width="200px">Naam</th><th>Tijd</th></tr>
        <?php
        $last5_runners_q="SELECT runner.fname, runner.lname, round.s_time,round.e_time,round.round_id FROM runner RIGHT JOIN round ON runner.student_id=round.student_id ORDER BY round_id DESC LIMIT 5";
        $last5_runners_x=mysqli_query($cxn,$last5_runners_q);
        while($runner_row=mysqli_fetch_assoc($last5_runners_x))
        {
            extract($runner_row);
            $round_time=strtotime($e_time)-strtotime($s_time);
            echo"<tr><td>{$fname} {$lname}</td><td>";minsec($round_time);echo"</td></tr>";
        }
        ?>
    </table>
    <img src="./images/24loopT-shirt4.png" width="105%" style="margin-left: -30px; margin-top: -8px;">

<footer>
    <p>Copyright &copy De Raedemaeker Stefan 2016-2017</p>
    <p>In opdracht van <a href="http://www.industria.be">INDUSTRIA vzw</a><small style="margin-left: 10px; color: rgba(0, 0, 0, 0.71)"><?php echo $version?></small></p>
</footer>

</div>
