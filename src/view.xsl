<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
>

  <xsl:import href="logo.xsl"/>

  <xsl:output
    method="xml"
    doctype-system="http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd"
    doctype-public="-//W3C//DTD XHTML 1.1//EN"
  />


  <!-- where are we right now? -->
  <xsl:variable name="current-scheme" select="/view/@scheme"/>
  <xsl:variable name="current-sort" select="/view/@sort"/>
  <xsl:variable name="current-projection" select="/view/@projection"/>
  <xsl:variable name="current-view" select="concat($current-scheme, '_', $current-sort, '_', $current-projection)"/>


  <!-- main master template for generated page -->
  <xsl:template match="/">
    <xsl:variable name="title">
      Cooperative Bug Isolation Report: <br/><xsl:value-of select="$current-scheme"/> predicates
    </xsl:variable>
    <html>
      <head>
	<title><xsl:value-of select="$title"/></title>
	<link rel="stylesheet" href="view.css" type="text/css"/>
      </head>

      <body>
	<!-- generic page header -->
	<h1>
	  <xsl:copy-of select="$logo-icon"/>
	  <xsl:copy-of select="$title"/>
	</h1>

	<table class="views">
	  <xsl:apply-templates select="document('summary.xml')/experiment/schemes"/>
	  <xsl:apply-templates select="document('sorts.xml')/sorts"/>
	  <xsl:apply-templates select="document('projections.xml')/projections"/>
	  <tr>
	    <th>Go to:</th>
	    <td>
	      [<a href="summary.xml">report summary</a>]
	      [<a href="{$projectURL}">CBI web page</a>]
	    </td>
	  </tr>
	</table>

	<xsl:apply-templates/>

      </body>
    </html>
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
      <th>Sorted by:</th>
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


  <!-- row of links to other projections -->
  <xsl:template match="projections">
    <tr>
      <th>Projection:</th>
      <td><xsl:apply-templates/></td>
    </tr>
  </xsl:template>

  <!-- link to a single other projection -->
  <xsl:template match="projection">
    <xsl:call-template name="other-view">
      <xsl:with-param name="name" select="@name"/>
      <xsl:with-param name="projection" select="@name"/>
    </xsl:call-template>
  </xsl:template>


  <!-- a single link to an alternative view (different scheme or sort order) -->
  <xsl:template name="other-view">
    <xsl:param name="name"/>
    <xsl:param name="scheme" select="$current-scheme"/>
    <xsl:param name="sort" select="$current-sort"/>
    <xsl:param name="projection" select="$current-projection"/>

    <xsl:variable name="label" select="translate($name, ' ', '&#160;')"/>
    <xsl:variable name="target-view" select="concat($scheme, '_', $sort, '_', $projection)"/>

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


  <xsl:template match="view">
    <table class="predictors">
      <tr>
	<xsl:call-template name="view-headings"/>
      </tr>
      <xsl:apply-templates/>
    </table>
  </xsl:template>


  <!-- headings for the big predictor table -->
  <!-- factored out to make it easier to reuse in other stylesheets -->
  <xsl:template name="view-headings">
    <th/>
    <th>predicate</th>
    <th>function</th>
    <th>file:line</th>
  </xsl:template>


  <!-- a single retained predictor -->
  <xsl:template match="predictor">
    <tr>
      <xsl:call-template name="predictor-cells"/>
    </tr>
  </xsl:template>


  <!-- the cells within a single retained predictor's row -->
  <!-- factored out to make it easier to reuse in other stylesheets -->
  <xsl:template name="predictor-cells">
    <td>
      <table class="scores" width="{scores/@log10-seen * 60 + 1}px" title="Ctxt: {round(scores/@context * 100)}%, LB: {round(scores/@lower-bound * 100)}%, Incr: {round(scores/@increase * 100)}%, Fail: {round(scores/@badness * 100)}%&#10;tru in {true/@success} S and {true/@failure} F&#10;obs in {seen/@success} S and {seen/@failure} F">
	<tr>
	  <td class="f1" style="width: {scores/@context * 100}%"/>
	  <td class="f2" style="width: {scores/@lower-bound * 100}%"/>
	  <td class="f3" style="width: {(scores/@badness - (scores/@context + scores/@lower-bound)) * 100}%"/>
	  <td class="f4" style="width: {(1 - scores/@badness) * 100}%"/>
	</tr>
      </table>
    </td>
    <xsl:apply-templates select="source"/>
  </xsl:template>


  <!-- source information for a single predictor -->
  <xsl:template match="source">
    <td>
      <xsl:value-of select="translate(@predicate, ' ', '&#160;')"/>
    </td>
    <td>
      <xsl:value-of select="@function"/>
    </td>
    <td>
      <xsl:variable name="source-dir" select="document('summary.xml')/experiment/@source-dir"/>
      <xsl:variable name="prefix">
	<xsl:choose>
	  <xsl:when test="starts-with(@file, '/')"/>
	  <xsl:when test="not($source-dir)"/>
	  <xsl:otherwise>
	    <xsl:value-of select="concat($source-dir, '/')"/>
	  </xsl:otherwise>
	</xsl:choose>
      </xsl:variable>
      <a href="{$prefix}{@file}#{@line}">
	<xsl:value-of select="@file"/>:<xsl:value-of select="@line"/>
      </a>
    </td>
  </xsl:template>


</xsl:stylesheet>
