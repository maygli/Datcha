function ipFilter(event)
{
    let charCode = (event.which) ? event.which : event.keyCode
    if (charCode != 46 && charCode != 110 && charCode != 190 && charCode > 31 
      && (charCode < 48 || charCode > 57))
       return false;
    return true;
}



