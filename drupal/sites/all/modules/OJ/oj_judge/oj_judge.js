$(document).ready(function()
{
	//alert($("#edit-type-2").attr("checked") == "checked");
	if($("#edit-type-0").attr("checked") == "checked")
	{
		
		$('.field-sid').show();
		$('.field-pid').hide();
		$('#edit-cid-wrapper').hide();
		$('.field-result').parent().hide();
	}
	if($("#edit-type-1").attr("checked") == "checked")
	{
		$('.field-sid').hide();
		$('.field-pid').show();
		$('#edit-cid-wrapper').hide();
		$('.field-result').parent().show();
	}
	if($("#edit-type-2").attr("checked") == "checked")
	{
		$('.field-sid').hide();
		$('.field-pid').hide();
		$('#edit-cid-wrapper').show();
		$('.field-result').parent().show();
	}
	
	$('#edit-result-0').click(function()
	{
		var isChecked = this.checked;
		var elements = $('#oj-rejudge-form :checkbox');
		var counter = elements.length;
		for(i = 0;i < counter;i++)
		{
			elements[i].checked = isChecked;
		}
	});
	
	$('#edit-type-0').click(function()
	{
		$('.field-sid').show();
		$('.field-pid').hide();
		$('#edit-cid-wrapper').hide();
		$('.field-result').parent().hide();
	});
	$('#edit-type-1').click(function()
	{
		$('.field-sid').hide();
		$('.field-pid').show();
		$('#edit-cid-wrapper').hide();
		$('.field-result').parent().show();
	});
	$('#edit-type-2').click(function()
	{
		$('.field-sid').hide();
		$('.field-pid').hide();
		$('#edit-cid-wrapper').show();
		$('.field-result').parent().show();
	});
	
	$("div[id^='edit-pid']").each(function(i)
	{
		$(this).append("<a class=remove-pid href=javascript:void(0)>Remove</a>");
	});
	$(".add_pid").click(function()
	{
		addRow('pid');
	});
	$(".remove-pid").live("click",function()
	{
		$(this).parent().remove();
	});
	
	$("div[id^='edit-sid']").each(function(i)
	{
		$(this).append("<a class=remove-sid href=javascript:void(0)>Remove</a>");
	});
	$(".add_sid").click(function()
	{
		addRow('sid');
	});
	$(".remove-sid").live("click",function()
	{
		$(this).parent().remove();
	});
});

function addRow(type)
{
	var last = $("div[id^='edit-"+type+"']").last();
	var i = 0;
	//alert(last.html());
	if(last.html() != null)
	{
		if(type == 'pid')
			var regx = /pid-(\d+)/;
		else
			var regx = /sid-(\d+)/;
		var rs = regx.exec(last.html());
		i = parseInt(rs[1])+1;
		$(last).after('<div id="edit-'+type+'-'+i+'-wrapper" class="form-item"><input type="text" class="form-text" value="" size="20" id="edit-'+type+'-'+i+'" name="'+type+'-'+i+'" maxlength="128">'+"\n"+'<a href="javascript:void(0)" class="remove-'+type+'">Remove</a></div>');
	}
	else
	{
		$(".add_"+type).parent().children("div").after('<div id="edit-'+type+'-'+i+'-wrapper" class="form-item"><input type="text" class="form-text" value="" size="20" id="edit-'+type+'-'+i+'" name="'+type+'-'+i+'" maxlength="128">'+"\n"+'<a href="javascript:void(0)" class="remove-'+type+'">Remove</a></div>');
	}
}
