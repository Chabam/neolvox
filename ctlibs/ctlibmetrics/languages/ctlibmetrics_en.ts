<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>CT_AbstractMetric</name>
    <message>
        <source> : </source>
        <translation> : </translation>
    </message>
    <message>
        <source>Impossible de trouver l&apos;attribut recherché (métrique : %2) !</source>
        <translation>Impossible to find the required attribute (metric: %2)!</translation>
    </message>
    <message>
        <source>Mauvais type pour l&apos;attribut recherché (%1 au lieu de %2) !</source>
        <translation>Wrong type for the searched attribute (%1 instead of %2)!</translation>
    </message>
</context>
<context>
    <name>CT_CloudMetrics</name>
    <message>
        <source>Indicateurs statistiques standards (Ht)</source>
        <translation>Standard statistical indicators (Ht)</translation>
    </message>
    <message>
        <source>Calcul des indicateurs statistiques standards, tels que le minimum, la maximum, la moyenne, l&apos;écart-type,...&lt;br&gt;Ces calculs sont effectués sur la coordonnée Z des points.&lt;br&gt;Cependant ils n&apos;ont d&apos;intérêt que pour un nuage de points en Hauteur, et non en Altitude.&lt;br&gt;Il faut donc s&apos;assurer que le nuage de points a été pré-traité, pour soustraire l&apos;altitude du sol à tous les points.</source>
        <translation>Calculation of standard statistical indicators, such as minimum, maximum, mean, standard deviation,...&lt;br&gt;These calculations are performed on the Z-coordinate of the points.&lt;br&gt;However, they are only of interest for a point cloud in Height, not in Altitude.&lt;br&gt;It is therefore necessary to ensure that the point cloud has been pre-processed, to subtract the ground elevation from all points.</translation>
    </message>
    <message>
        <source>Les indicateurs suivants sont calculés :&lt;ul&gt;&lt;li&gt;&lt;strong&gt;N&lt;/strong&gt; : Nombre de points&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Min&lt;/strong&gt; : Hauteur minimum&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Max&lt;/strong&gt; : Hauteur maximum&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mean&lt;/strong&gt; : Hauteur moyenne&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mode&lt;/strong&gt; : Mode des hauteurs. La gamme de hauteur est divisée en 64 classes, le mode est le centre de la classe avec le plus grand nombre de points. En cas d&apos;égalité, la classe la plus basse est renvoyée.&lt;/li&gt;&lt;li&gt;&lt;strong&gt;StdDev&lt;/strong&gt; : Ecart-type des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Variance&lt;/strong&gt; : Variance des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CV&lt;/strong&gt; : Coefficient de variation des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;IQDist&lt;/strong&gt; : Distance interquartile des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Skewness&lt;/strong&gt; : Skewness des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Kurtosis&lt;/strong&gt; : Kurtosis des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;AAD&lt;/strong&gt; : Moyenne des écarts absolus à la moyenne&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MAD_Median&lt;/strong&gt; : Médiane des écarts absolus à la médiane&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MAD_Mode&lt;/strong&gt; : Médiane des écarts absolus au mode&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L1 à L4&lt;/strong&gt; : L-Moments&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L_CV&lt;/strong&gt; : Coefficient de variation L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L_Skewness&lt;/strong&gt; : Skewness L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L_Kurtosis&lt;/strong&gt; : Kurtosis L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;P01 à P99&lt;/strong&gt; : 1er, 5ième, 10ième, 15ième,... 90ième, 95ième et 99ième percentiles de hauteurs (interpolation linéaire entre les observations)&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Canopy_Relief_Ratio&lt;/strong&gt; : ((Moyenne - Minimum) / (Maximum – Minimum))&lt;/li&gt;&lt;li&gt;&lt;strong&gt;QuadraticMean&lt;/strong&gt; : Moyenne quadratique des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CubicMean&lt;/strong&gt; : Moyenne cubique des hauteurs&lt;/li&gt;&lt;/ul&gt;</source>
        <translation type="vanished">The following indicators are calculated:&lt;ul&gt;&lt;li&gt;&lt;strong&gt;N&lt;/strong&gt; : Number of points&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Min&lt;/strong&gt; : Minimum height&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Max&lt;/strong&gt; : Maximum height&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mean&lt;/strong&gt; : Average height&lt;/li&gt;&lt;strong&gt;Mode&lt;/strong&gt; : Mode of heights. The height range is divided into 64 classes, the mode is the centre of the class with the highest number of points. In case of a tie, the lowest class is returned.&lt;/li&gt;&lt;li&gt;&lt;strong&gt;StdDev&lt;/strong&gt; : Standard deviation of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Variance&lt;/strong&gt; : Variance of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CV&lt;/strong&gt; : Coefficient of variation of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;IQDist&lt;/strong&gt; : Interquartile distance of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Skewness&lt;/strong&gt; : Skewness of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Kurtosis&lt;/strong&gt; : Kurtosis of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;AAD&lt;/strong&gt; : Mean of absolute deviations from the mean&lt;/li&gt;&lt;strong&gt;MAD_Median&lt;/strong&gt; : Median of absolute deviations from the median&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MAD_Mode&lt;/strong&gt;: Median of absolute deviations from the mode&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L1 to L4&lt;/strong&gt;: L-Moments&lt;/li&gt;&lt;strong&gt;L_CV&lt;/strong&gt;:  L-Moments coefficient of variationof heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L_Skewness&lt;/strong&gt; : L-Moments Skewness of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L_Kurtosis&lt;/strong&gt; : L-Moments Kurtosis of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;P01 to P99&lt;/strong&gt; : 1st, 5th, 10th, 15th,... 90th, 95th and 99th percentiles of heights (linear interpolation between observations)&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Canopy_Relief_Ratio&lt;/strong&gt; : ((Mean - Minimum) / (Maximum - Minimum))&lt;/li&gt;&lt;li&gt;&lt;strong&gt;QuadraticMean&lt;/strong&gt; : Quadratic mean of heights&lt;/li&gt;&lt;strong&gt;CubicMean&lt;/strong&gt; : Cubic mean of heights&lt;/li&gt;&lt;/ul&gt;</translation>
    </message>
    <message>
        <source>Ne conserver que les points avec H &gt;= </source>
        <translation>Keep only points with H &gt;= </translation>
    </message>
    <message>
        <source>N</source>
        <translation type="vanished">N</translation>
    </message>
    <message>
        <source>Min</source>
        <translation type="vanished">Min</translation>
    </message>
    <message>
        <source>Max</source>
        <translation type="vanished">Max</translation>
    </message>
    <message>
        <source>Mean</source>
        <translation type="vanished">Mean</translation>
    </message>
    <message>
        <source>Mode</source>
        <translation type="vanished">Mode</translation>
    </message>
    <message>
        <source>StdDev</source>
        <translation type="vanished">StdDev</translation>
    </message>
    <message>
        <source>Variance</source>
        <translation type="vanished">Variance</translation>
    </message>
    <message>
        <source>CV</source>
        <translation type="vanished">CV</translation>
    </message>
    <message>
        <source>IQDist</source>
        <translation type="vanished">IQDist</translation>
    </message>
    <message>
        <source>Skewness</source>
        <translation type="vanished">Skewness</translation>
    </message>
    <message>
        <source>Kurtosis</source>
        <translation type="vanished">Kurtosis</translation>
    </message>
    <message>
        <source>AAD</source>
        <translation type="vanished">AAL</translation>
    </message>
    <message>
        <source>MAD_Median</source>
        <translation type="vanished">MAD_Median</translation>
    </message>
    <message>
        <source>MAD_Mode</source>
        <translation type="vanished">MAD_Mode</translation>
    </message>
    <message>
        <source>L%1</source>
        <translation type="vanished">L%1</translation>
    </message>
    <message>
        <source>L_CV</source>
        <translation type="vanished">L_CV</translation>
    </message>
    <message>
        <source>L_Skewness</source>
        <translation type="vanished">L_Skewness</translation>
    </message>
    <message>
        <source>L_Kurtosis</source>
        <translation type="vanished">L_Kurtosis</translation>
    </message>
    <message>
        <source>P%1%2</source>
        <translation type="vanished">P%1%2</translation>
    </message>
    <message>
        <source>Canopy_Relief_Ratio</source>
        <translation type="vanished">Canopy_Relief_Ratio</translation>
    </message>
    <message>
        <source>QuadraticMean</source>
        <translation type="vanished">QuadraticMean</translation>
    </message>
    <message>
        <source>CubicMean</source>
        <translation type="vanished">CubicMean</translation>
    </message>
    <message>
        <source>Les indicateurs suivants sont calculés :&lt;ul&gt;&lt;li&gt;&lt;strong&gt;N&lt;/strong&gt; : Nombre de points&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Min&lt;/strong&gt; : Hauteur minimum&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Max&lt;/strong&gt; : Hauteur maximum&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mean&lt;/strong&gt; : Hauteur moyenne&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mode&lt;/strong&gt; : Mode des hauteurs. La gamme de hauteur est divisée en 64 classes, le mode est le centre de la classe avec le plus grand nombre de points. En cas d&apos;égalité, la classe la plus basse est renvoyée.&lt;/li&gt;&lt;li&gt;&lt;strong&gt;StdDev&lt;/strong&gt; : Ecart-type des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Var&lt;/strong&gt; : Variance des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CV&lt;/strong&gt; : Coefficient de variation des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;IQDist&lt;/strong&gt; : Distance interquartile des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Skew&lt;/strong&gt; : Skewness des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Kurt&lt;/strong&gt; : Kurtosis des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;AAD&lt;/strong&gt; : Moyenne des écarts absolus à la moyenne&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MADMed&lt;/strong&gt; : Médiane des écarts absolus à la médiane&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MADMode&lt;/strong&gt; : Médiane des écarts absolus au mode&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L1 à L4&lt;/strong&gt; : L-Moments&lt;/li&gt;&lt;li&gt;&lt;strong&gt;LCV&lt;/strong&gt; : Coefficient de variation L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;LSkew&lt;/strong&gt; : Skewness L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;LKurt&lt;/strong&gt; : Kurtosis L-Moments des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;P01 à P99&lt;/strong&gt; : 1er, 5ième, 10ième, 15ième,... 90ième, 95ième et 99ième percentiles de hauteurs (interpolation linéaire entre les observations)&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CRR&lt;/strong&gt; : ((Moyenne - Minimum) / (Maximum – Minimum))&lt;/li&gt;&lt;li&gt;&lt;strong&gt;QuadMean&lt;/strong&gt; : Moyenne quadratique des hauteurs&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CubMean&lt;/strong&gt; : Moyenne cubique des hauteurs&lt;/li&gt;&lt;/ul&gt;</source>
        <translation>The following indicators are calculated:&lt;ul&gt;&lt;li&gt;&lt;strong&gt;N&lt;/strong&gt; : Number of points&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Min&lt;/strong&gt; : Minimum height&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Max&lt;/strong&gt; : Maximum height&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Mean&lt;/strong&gt; : Average height&lt;/li&gt;&lt;strong&gt;Mode&lt;/strong&gt; : Mode of heights. The height range is divided into 64 classes, the mode is the centre of the class with the highest number of points. In case of a tie, the lowest class is returned.&lt;/li&gt;&lt;li&gt;&lt;strong&gt;StdDev&lt;/strong&gt; : Standard deviation of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Var&lt;/strong&gt; : Variance of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CV&lt;/strong&gt; : Coefficient of variation of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;IQDist&lt;/strong&gt; : Interquartile distance of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Skew&lt;/strong&gt; : Skewness of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;Kurt&lt;/strong&gt; : Kurtosis of heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;AAD&lt;/strong&gt; : Mean of absolute deviations from the mean&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MADMed&lt;/strong&gt; : Median of absolute deviations from the median&lt;/li&gt;&lt;li&gt;&lt;strong&gt;MADMode&lt;/strong&gt;: Median of absolute deviations from the mode&lt;/li&gt;&lt;li&gt;&lt;strong&gt;L1 to L4&lt;/strong&gt;: L-Moments&lt;/li&gt;&lt;strong&gt;LCV&lt;/strong&gt;: Coefficient of variation L-Moments of the heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;LSkew&lt;/strong&gt; : Skewness L-Moments of the heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;LKurt&lt;/strong&gt; : Kurtosis L-Moments of the heights&lt;/li&gt;&lt;li&gt;&lt;strong&gt;P01 to P99&lt;/strong&gt; : 1st, 5th, 10th, 15th,... 90th, 95th and 99th percentiles of heights (linear interpolation between observations)&lt;/li&gt;&lt;li&gt;&lt;strong&gt;CRR&lt;/strong&gt; : ((Mean - Minimum) / (Maximum - Minimum))&lt;/li&gt;&lt;li&gt;&lt;strong&gt;QuadMean&lt;/strong&gt; : Quadratic mean of the heights&lt;/li&gt;&lt;strong&gt;CubMean&lt;/strong&gt; : Cubic mean of the heights&lt;/li&gt;&lt;/ul&gt;</translation>
    </message>
</context>
</TS>
