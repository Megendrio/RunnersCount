<?php $version=0.1?>
<head>
    <title>Industria 24 urenloop</title>
    <style>
        body
        {
            margin: 0px;
            padding: 0px;
            width: 100%;
            font-family: "Segoe UI", sans-serif;
        }
        a
        {
            color: #24499f;
        }
        #frame
        {
            width: 600px;
            height: auto;
            margin-left: auto;
            margin-right: auto;
            margin-top: 100px;
            text-align: center;
            border: 1px solid #24499f;
            background-color: #ebebeb;
            border-radius: 5px;
        }
        footer
        {
            position: fixed;
            bottom:0;
            margin: 20px -1px 0px -1px;
            width: 100%;
            border-width: 1px;
            background-color: #ebebeb;
            font-size: 0.8em;
            line-height: 0.6em;
            text-align: center;
        }
    </style>
</head>

<body>
<div id="frame">
    <h1>Industria 24 urenloop</h1>
    <img src="images/industria_shield_logo.png" height="80px">
    <h2>Webpagina's</h2>
    <p><a href="RunnerSetup.php">Runner Setup</a>: Administratie lopers</p>
    <p><a href="RunnerQueue.php">Runner Queue</a>: Toon scherm aan toeschouwers</p>
    <p><a href="RunnerShow.php">Runner Show</a>: Toon scherm aan toeschouwers</p>
</div>

<footer>
    <p>Copyright &copy De Raedemaeker Stefan 2016-2017</p>
    <p>In opdracht van <a href="http://www.industria.be">INDUSTRIA vzw</a><small style="margin-left: 10px; color: rgba(0, 0, 0, 0.71)"><?php echo $version?></small></p>
</footer>

</body>