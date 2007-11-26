if (navigator.appName == "Microsoft Internet Explorer")
  window.onload = function()
    {
      var tables = document.getElementsByTagName("table");
      for (var i = 0; i < tables.length; ++i)
      if (tables[i].className == "meter")
	{
	  var meter = tables[i].parentNode;
	  meter.onmouseover = function()
	    {
	      this.className = "meter meter-hover";
	    }
	  meter.onmouseout = function()
	    {
	      this.className = "meter";
	    }
	}
    }
