<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="scores.xsl"/>

  <xsl:import href="logo.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>


  <!-- where are we right now? -->
  <xsl:variable name="current-scheme" select="/scores/@scheme"/>
  <xsl:variable name="current-sort" select="/scores/@sort"/>
  <xsl:variable name="current-view" select="concat($current-scheme, '_', $current-sort, '_none')"/>


  <!-- customized page CSS stylesheet -->
  <xsl:template match="/" mode="css">
    view.css
  </xsl:template>


  <!-- customized page title -->
  <xsl:template match="/" mode="title">
    Cooperative Bug Isolation Report: <xsl:value-of select="$current-scheme"/> predicates
  </xsl:template>


  <!-- customized page header -->
  <xsl:template match="/" mode="page-header">
    <xsl:apply-imports/>
    <table class="views">
      <xsl:apply-templates select="document('summary.xml')/experiment/schemes"/>
      <xsl:apply-templates select="document('sorts.xml')/sorts"/>
      <tr>
	<th>Go to:</th>
	<td>
	  <xsl:apply-templates select="scores" mode="go-to-links"/>
	</td>
      </tr>
    </table>
  </xsl:template>


  <!-- row of links to other schemes' pages -->
  <xsl:template match="schemes">
    <tr>
      <th>Scheme:</th>
      <td>
	<xsl:apply-templates/>
	<xsl:call-template name="other-view">
	  <xsl:with-param name="name" select="'all'"/>
	  <xsl:with-param name="scheme" select="'all'"/>
	  <xsl:with-param name="sort" select="$current-sort"/>
	</xsl:call-template>
      </td>
    </tr>
  </xsl:template>

  <!-- link to a single other scheme's page -->
  <xsl:template match="scheme[@retain > 0]">
    <xsl:call-template name="other-view">
      <xsl:with-param name="name" select="@name"/>
      <xsl:with-param name="scheme" select="@name"/>
    </xsl:call-template>
  </xsl:template>


  <!-- row of links to other sort orders -->
  <xsl:template match="sorts">
    <tr>
      <th>Sorted&#160;by:</th>
      <td><xsl:apply-templates/></td>
    </tr>
  </xsl:template>

  <!-- link to a single other sort order -->
  <xsl:template match="sort">
    <xsl:call-template name="other-view">
      <xsl:with-param name="name" select="text()"/>
      <xsl:with-param name="sort" select="@code"/>
    </xsl:call-template>
  </xsl:template>


  <!-- a single link to an alternative view (different scheme or sort order) -->
  <xsl:template name="other-view">
    <xsl:param name="name"/>
    <xsl:param name="scheme" select="$current-scheme"/>
    <xsl:param name="sort" select="$current-sort"/>

    <xsl:variable name="label" select="translate($name, ' ', '&#160;')"/>
    <xsl:variable name="target-view" select="concat($scheme, '_', $sort, '_none')"/>

    <!-- output a placeholder or a true link -->
    [<xsl:choose>
      <xsl:when test="$target-view = $current-view">
	<!-- placeholder for the link to ourself -->
	<xsl:value-of select="$label"/>
      </xsl:when>
      <xsl:otherwise>
	<!-- true link to someone else -->
	<a href="{$target-view}.xml"><xsl:value-of select="$label"/></a>
      </xsl:otherwise>
    </xsl:choose>]
  </xsl:template>


  <!-- miscellaneous handy links to related pages -->
  <xsl:template match="scores" mode="go-to-links">
    [<a href="summary.xml">report summary</a>]
    [<a href="{$projectURL}">CBI web page</a>]
  </xsl:template>


  <!-- custom headings for the big predictor table -->
  <xsl:template match="scores" mode="dynamic-headings">
    <th>Thermometer</th>
  </xsl:template>


  <!-- custom data cells for each predictor row in the big table -->
  <xsl:template match="predictor" mode="dynamic-cells">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates select="document('predictor-info.xml')/predictor-info/info[$index]/bug-o-meter"/>
  </xsl:template>


</xsl:stylesheet>
