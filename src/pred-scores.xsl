<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>

  <!-- customized page CSS stylesheet -->
  <xsl:template match="/" mode="css">
    pred-scores.css
  </xsl:template>


  <!-- extra column headings -->
  <xsl:template match="scores" mode="dynamic-headings">
    <xsl:apply-imports/>	
    <th>Thermometer</th>
    <th> stat1 </th>	     	
    <th> stat2 </th>
  </xsl:template>

  <!-- extra predictor row cells -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <xsl:apply-imports/>
    <xsl:variable name="index" select="number(@index)"/>
    <td class="meter">
    <xsl:apply-templates select="document('predictor-info.xml')/predictor-info/info[$index]/bug-o-meter"/>
    </td>
    <td><xsl:value-of select="@stat1"/></td>
    <td><xsl:value-of select="@stat2"/></td>
  </xsl:template>

   <xsl:template match="conjunction" mode="dynamic-cells">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:variable name="num" 
	    select="count(document('conjunction-info.xml', /)/conjunction-info/info[@index= $index]/pred)"/>
    <td rowspan="{$num}" class="meter">
   	 <xsl:apply-templates 
		 select="document('conjunction-info.xml')/conjunction-info/info[@index= $index]/bug-o-meter"/>
 	 </td> 
	 <td rowspan="{$num}"><xsl:value-of select="@stat1"/></td>
	 <td rowspan="{$num}"><xsl:value-of select="@stat2"/></td>
  </xsl:template>

</xsl:stylesheet>
