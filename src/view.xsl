<?xml version="1.0"?>
<!DOCTYPE stylesheet>

<xsl:stylesheet
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml"
  >

  <xsl:import href="logo.xsl"/>
  <xsl:import href="bug-o-meter.xsl"/>

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
      Cooperative Bug Isolation Report: <xsl:value-of select="$current-scheme"/> predicates
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
	      <xsl:apply-templates mode="go-to-links" select="view"/>
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


  <!-- handy links to related pages -->
  <xsl:template mode="go-to-links" match="view">
    [<a href="summary.xml">report summary</a>]
    [<a href="{$projectURL}">CBI web page</a>]
  </xsl:template>


  <!-- the bug predictor table -->
  <xsl:template match="view">
    <table class="predictors">
      <thead>
	<tr>
	  <xsl:apply-templates mode="dynamic-headings" select="."/>
	  <xsl:apply-templates mode="static-headings" select="."/>
	</tr>
      </thead>
      <tbody>
	<xsl:apply-templates/>
      </tbody>
    </table>
  </xsl:template>


  <!-- headings for the big predictor table -->
  <xsl:template mode="dynamic-headings" match="view">
    <th>Bug-O-Meter&#8482;</th>
  </xsl:template>

  <xsl:template mode="static-headings" match="view">
    <th>Predicate</th>
    <th>Function</th>
    <th>File:Line</th>
  </xsl:template>


  <!-- row for a single retained predictor -->
  <xsl:template match="predictor">
    <tr>
      <xsl:attribute name="title">rank: <xsl:number/>, pred: <xsl:value-of select="@index"/></xsl:attribute>
      <xsl:apply-templates mode="dynamic-cells" select="."/>
      <xsl:apply-templates mode="static-cells" select="."/>
    </tr>
  </xsl:template>


  <!-- multicolored bug thermometer -->
  <xsl:template mode="dynamic-cells" match="predictor">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates select="document('predictor-info.xml')/predictor-info/info[$index]/bug-o-meter"/>
  </xsl:template>


  <!-- several cells depending only on static predicate information -->
  <xsl:template mode="static-cells" match="predictor">
    <xsl:variable name="index" select="number(@index)"/>
    <xsl:apply-templates mode="static-cells" select="document('predictor-info.xml')/predictor-info/info[$index]"/>
  </xsl:template>

  <xsl:template mode="static-cells" match="info">
    <td>
      <xsl:apply-templates mode="operands" select="."/>
    </td>
    <td><xsl:value-of select="@function"/>()</td>
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


  <!-- static information for a single predicate -->
  <xsl:template mode="operands" match="info[@scheme = 'branches' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> is FALSE
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'branches' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> is TRUE
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'g-object-unref' and @predicate = 0]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) == 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'g-object-unref' and @predicate = 1]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) == 1
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'g-object-unref' and @predicate = 2]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) &gt; 1
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'g-object-unref' and @predicate = 3]">
    old_refcount(<xsl:value-of select="operand[1]/@source"/>) is invalid
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> &lt; 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> &gt;= 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 2]">
    <xsl:value-of select="operand[1]/@source"/> == 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 3]">
    <xsl:value-of select="operand[1]/@source"/> != 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 4]">
    <xsl:value-of select="operand[1]/@source"/> &gt; 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'returns' and @predicate = 5]">
    <xsl:value-of select="operand[1]/@source"/> &lt;= 0
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 0]">
    <xsl:value-of select="operand[1]/@source"/> &lt; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 1]">
    <xsl:value-of select="operand[1]/@source"/> &gt;= <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 2]">
    <xsl:value-of select="operand[1]/@source"/> == <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 3]">
    <xsl:value-of select="operand[1]/@source"/> != <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 4]">
    <xsl:value-of select="operand[1]/@source"/> &gt; <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info[@scheme = 'scalar-pairs' and @predicate = 5]">
    <xsl:value-of select="operand[1]/@source"/> &lt;= <xsl:value-of select="operand[2]/@source"/>
  </xsl:template>

  <xsl:template mode="operands" match="info">
    <xsl:message terminate="yes">
      Cannot format source for scheme <xsl:value-of select="@scheme"/>, predicate <xsl:value-of select="@predicate"/>.
    </xsl:message>
  </xsl:template>

  <xsl:template mode="operands" match="*">
    <xsl:message terminate="yes">
      Cannot format source for tag "<xsl:value-of select="name()"/>".
    </xsl:message>
  </xsl:template>


</xsl:stylesheet>
