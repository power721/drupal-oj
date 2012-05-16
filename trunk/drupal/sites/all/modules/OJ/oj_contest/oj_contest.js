$(document).ready(function(){

	var dates = $("#edit-start-time-date, #edit-end-time-date").datepicker({
		dateFormat: 'yy-mm-dd',
		maxDate: +356,
		showAnim: 'blind',
		changeMonth: true,
		changeYear: true,
		onSelect: function( selectedDate ) {
				var option = this.id == "edit-start-time-date" ? "minDate" : "maxDate",
					instance = $( this ).data( "datepicker" ),
					date = $.datepicker.parseDate(
						instance.settings.dateFormat ||
						$.datepicker._defaults.dateFormat,
						selectedDate, instance.settings );
				dates.not( this ).datepicker( "option", option, date );
			}
	});
	/*
	$("input[name$='[pid]']").live("blur",function(){
		var name = $(this).attr('name');
		var pid = this.value;
		var regex = /problems\[(.*)\]\[pid\]/;
		var num = regex.exec(name)[1];
		$("#problem-title-"+num).load("/drupal/contest_problem_title.ajax/"+pid);
	});
	*/
});