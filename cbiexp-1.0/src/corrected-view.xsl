<?xml version="1.0"?>
<!DOCTYPE stylesheet [
  <!ENTITY link "&#9679;">
]>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>


  <!-- customized page CSS stylesheet -->
  <xsl:template match="/" mode="css">
    view.css
  </xsl:template>


  <!-- customized dynamic data column headers for the big table -->
  <xsl:template match="scores" mode="dynamic-headings">
    <th>Initial Score</th>
    <th>Initial Thermometer</th>
    <th>Effective Score</th>
    <th>Effective Thermometer</th>
    <th>Zoom</th>
  </xsl:template>


  <!-- customized dynamic data cells for each predictor row -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <td><xsl:value-of select="@initial"/></td>
    <xsl:variable name="index" select="number(@index)"/>
    <td class="meter">
    <xsl:apply-templates select="document('predictor-info.xml', /)/predictor-info/info[$index]/bug-o-meter"/>
    </td>	
    <td><xsl:value-of select="@effective"/></td>
    <td class="meter"><xsl:apply-templates select="bug-o-meter"/></td>
    <td class="link">
      <a href="zoom-{/scores/@projection}-{@index}.xml">
	&link;
      </a>
    </td>
  </xsl:template>


  <!-- customized dynamic data cells for each predictor row -->
  <xsl:template match="conjunction">
    <xsl:variable name="num" 
	    select="count(./pred)"/>	
    <tr>	
    <td rowspan= "{$num}"><xsl:value-of select="@initial"/></td>
    
    <td rowspan= "{$num}" class="meter">
    <xsl:apply-templates select="bug-o-meter[1]"/>
    </td>	
    <td rowspan= "{$num}"><xsl:value-of select="@effective"/></td>
    <td rowspan= "{$num}" class="meter"><xsl:apply-templates select="bug-o-meter[2]"/></td>
    <td rowspan= "{$num}" class="link">
      <a href="zoom-111.xml">
	&link;
      </a>
    </td>
     <xsl:for-each
	      select="./pred[1]">
	      <xsl:variable name="i" select="number(@index)"/>
	      <xsl:variable name="info" select="document('predictor-info.xml', /)/predictor-info/info[$i]"/>
	      <xsl:apply-templates select="$info" mode="static-cells"/>
      </xsl:for-each>	
    </tr>

    <xsl:for-each
   select="./pred[position() > 1]">
	      <xsl:variable name="i" select="number(@index)"/>
	      <xsl:variable name="info" select="document('predictor-info.xml', /)/predictor-info/info[$i]"/>
	      <tr> <xsl:apply-templates select="$info" mode="static-cells"/> </tr>
      </xsl:for-each>

  </xsl:template>

  


</xsl:stylesheet>
