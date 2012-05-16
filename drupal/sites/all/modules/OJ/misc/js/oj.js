// increase the default animation speed to exaggerate the effect
	$.fx.speeds._default = 1000;
	$(function() {
	
		$('div#contactable').toggle(function() {
				
                $(this).animate({"marginLeft": "-=5px"}, "fast");
                $('#sidebar-left').animate({"marginLeft": "-=0px"}, "fast");
                $(this).animate({"marginLeft": "+=190px"}, "slow");
                $('#sidebar-left').animate({"marginLeft": "+=400px"}, "slow");
            },
            function() {
                $('#sidebar-left').animate({"marginLeft": "-=400px"}, "slow");
                $(this).animate({"marginLeft": "-=190px"}, "slow").animate({"marginLeft": "+=5px"}, "fast");
            });
		
		$("#oj-login").dialog({
			autoOpen: false,
			show: "blind",
			hide: "explode",
			width: 230
		});

		$("#login").click(function() {
			$("#oj-login").dialog( "open" );
			return false;
		});
		
		/*$("#logout").click(function() {
			window.location="/drupal/logout";
		});*/
	});