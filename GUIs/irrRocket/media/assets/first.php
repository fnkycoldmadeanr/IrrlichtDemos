<rml>
	<head>
		<title>First page</title>
		<link type="text/template" href="window.rml" />
		<style>
			body
			{
				width: 400px;
				height: 325px;

				margin: auto;
			}

			div#title_bar div#icon
			{
				display: none;
			}

			div#content
			{
				text-align: left;
			}

            #navbar {
                display: block;
                width: 100%;
                height: 50px;
                background-color: #1976d2;
			}

			#dropdown-content {
                display: none;
                width: 150px;
                background-color: #1976d2;
                border-width: 3px;
                border-color: #004ba0;
                padding: 0px;
			}

            #button-second, #button-third {
                color: white;
                background-color: #1976d2;
                float: left;
                width: 150px;
                shadow-font-effect: shadow;
                shadow-offset: 1px 1px;
                shadow-color: black;
			}

			p, img {
                z-index: -1;
			}

			#textarea {
                border-width: 1px;
                background-color: white;
                color: black;
			}

		</style>
	</head>

	<body template="window">

        <div id="navbar">
            <button onclick="dropdown" id="dropdown-button" type="button">Dropdown</button>
            <div id="dropdown-content">
                <button onclick="second" id="button-second" type="button">Load second.rml</button>
                <button onclick="third" id="button-third" type="button">Load third.rml</button>
            </div>
        </div>

        <p>First RML</p>
        <?php
            echo "<p>Current date: ";
            echo date('Y-m-d H:i:s');
            echo "</p>";
         ?>

        <form id="form" onsubmit="input">
            <textarea id="textarea" cols="20" rows="2"/>
			<input type="submit">Print to console</input>
		</form>

        <p>Click on the image to load second.rml</p>
        <img src="clouds.jpg" onclick="second"/>

    </body>
</rml>

