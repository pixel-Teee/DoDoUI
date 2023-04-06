#include "PreCompileHeader.h"

#include "Color.h"

namespace DoDo
{
	const FLinearColor FLinearColor::White(1.0f, 1.0f, 1.0f);
	const FLinearColor FLinearColor::Gray(0.5f, 0.5f, 0.5f);
	const FLinearColor FLinearColor::Black(0.0f, 0.0f, 0.0f);
	const FLinearColor FLinearColor::Transparent(0.0f, 0.0f, 0.0f, 0.0f);
	const FLinearColor FLinearColor::Red(1.0f, 0.0f, 0.0f);
	const FLinearColor FLinearColor::Green(0.0f, 1.0f, 0.0f);
	const FLinearColor FLinearColor::Blue(0.0f, 0.0f, 1.0f);
	const FLinearColor FLinearColor::Yellow(1.0f, 1.0f, 0.0f);

	const FColor FColor::White(255, 255, 255);
	const FColor FColor::Black(0, 0, 0);
	const FColor FColor::Transparent(0, 0, 0, 0);
	const FColor FColor::Red(255, 0, 0);
	const FColor FColor::Green(0, 255, 0);
	const FColor FColor::Blue(0, 0, 255);
	const FColor FColor::Yellow(255, 255, 0);
	const FColor FColor::Cyan(0, 255, 255);//qing se
	const FColor FColor::Magenta(255, 0, 255);//pin hong
	const FColor FColor::Orange(243, 156, 18);
	const FColor FColor::Purple(169, 7, 228);//zi se
	const FColor FColor::Turquoise(26, 188, 156);//lv song shi, qing se
	const FColor FColor::Silver(189, 195, 199);//yin se
	const FColor FColor::Emerald(46, 204, 113);//fei cui

	// fast Linear to SRGB uint8 conversion
	// https://gist.github.com/rygorous/2203834
	//
	// round-trips exactly
	// quantization bucket boundaries vary by max of 0.11%
	// biggest difference at i = 1
	// thresholds[i] = 0.000456
	// c_linear_float_srgb_thresholds[i] = 0.000455

	typedef union
	{
		uint32_t u;
		float f;
	} stbir__FP32;

	static const uint32_t stb_fp32_to_srgb8_tab4[104] = {
		0x0073000d, 0x007a000d, 0x0080000d, 0x0087000d, 0x008d000d, 0x0094000d, 0x009a000d, 0x00a1000d,
		0x00a7001a, 0x00b4001a, 0x00c1001a, 0x00ce001a, 0x00da001a, 0x00e7001a, 0x00f4001a, 0x0101001a,
		0x010e0033, 0x01280033, 0x01410033, 0x015b0033, 0x01750033, 0x018f0033, 0x01a80033, 0x01c20033,
		0x01dc0067, 0x020f0067, 0x02430067, 0x02760067, 0x02aa0067, 0x02dd0067, 0x03110067, 0x03440067,
		0x037800ce, 0x03df00ce, 0x044600ce, 0x04ad00ce, 0x051400ce, 0x057b00c5, 0x05dd00bc, 0x063b00b5,
		0x06970158, 0x07420142, 0x07e30130, 0x087b0120, 0x090b0112, 0x09940106, 0x0a1700fc, 0x0a9500f2,
		0x0b0f01cb, 0x0bf401ae, 0x0ccb0195, 0x0d950180, 0x0e56016e, 0x0f0d015e, 0x0fbc0150, 0x10630143,
		0x11070264, 0x1238023e, 0x1357021d, 0x14660201, 0x156601e9, 0x165a01d3, 0x174401c0, 0x182401af,
		0x18fe0331, 0x1a9602fe, 0x1c1502d2, 0x1d7e02ad, 0x1ed4028d, 0x201a0270, 0x21520256, 0x227d0240,
		0x239f0443, 0x25c003fe, 0x27bf03c4, 0x29a10392, 0x2b6a0367, 0x2d1d0341, 0x2ebe031f, 0x304d0300,
		0x31d105b0, 0x34a80555, 0x37520507, 0x39d504c5, 0x3c37048b, 0x3e7c0458, 0x40a8042a, 0x42bd0401,
		0x44c20798, 0x488e071e, 0x4c1c06b6, 0x4f76065d, 0x52a50610, 0x55ac05cc, 0x5892058f, 0x5b590559,
		0x5e0c0a23, 0x631c0980, 0x67db08f6, 0x6c55087f, 0x70940818, 0x74a007bd, 0x787d076c, 0x7c330723,
	};

	static uint8_t stbir__linear_to_srgb_uchar_fast(float in)
	{
		static const stbir__FP32 almostone = { 0x3f7fffff }; // 1-eps
		static const stbir__FP32 minval = { (127 - 13) << 23 };
		uint32_t tab, bias, scale, t;
		stbir__FP32 f;

		// Clamp to [2^(-13), 1-eps]; these two values map to 0 and 1, respectively.
		// The tests are carefully written so that NaNs map to 0, same as in the reference
		// implementation.
		if (!(in > minval.f)) // written this way to catch NaNs
			in = minval.f;
		if (in > almostone.f)
			in = almostone.f;

		// Do the table lookup and unpack bias, scale
		f.f = in;
		tab = stb_fp32_to_srgb8_tab4[(f.u - minval.u) >> 20];
		bias = (tab >> 16) << 9;
		scale = tab & 0xffff;

		// Grab next-highest mantissa bits and perform linear interpolation
		t = (f.u >> 12) & 0xff;
		return (uint8_t)((bias + scale * t) >> 16);
	}

	static inline uint8_t clamp_u8(int32_t value)
	{
		return (uint8_t)std::clamp(value, 0, 255);
	}

	FLinearColor::FLinearColor(const FColor& color)
	{
		R = srgb_to_linear_table[color.R];
		G = srgb_to_linear_table[color.G];
		B = srgb_to_linear_table[color.B];
		A = float(color.A) * (1.0f / 255.0f);
	}

	/*quantizes the linear color and returns the result as a FColor with optional srgb conversion and quality as goal*/
	FColor FLinearColor::to_fcolor_srgb() const
	{
		return FColor(
			stbir__linear_to_srgb_uchar_fast(R),
			stbir__linear_to_srgb_uchar_fast(G),
			stbir__linear_to_srgb_uchar_fast(B),
			clamp_u8((int32_t)(0.5f + A * 255.0f))
		);
	}

	FColor FLinearColor::quantize_round() const
	{
		return FColor(
			clamp_u8((int)(0.5f + R * 255.f)),
			clamp_u8((int)(0.5f + G * 255.f)),
			clamp_u8((int)(0.5f + B * 255.f)),
			clamp_u8((int)(0.5f + A * 255.f))
		);
	}

	FColor FLinearColor::to_fcolor(const bool b_srgb) const
	{
		if(b_srgb)
		{
			return to_fcolor_srgb();
		}
		else
		{
			return quantize_round();
		}
	}

	FLinearColor FLinearColor::linear_rgb_to_hsv() const
	{
		const float rgb_min = std::min(R, std::min(G, B));
		const float rgb_max = std::max(R, std::max(G, B));
		const float rgb_range = rgb_max - rgb_min;

		const float hue = (rgb_max == rgb_min ? 0.0f :
							rgb_max == R ? std::fmod((((G - B) / rgb_range) * 60.0f) + 360.0f, 360.0f) :
							rgb_max == G ? (((B - R) / rgb_range) * 60.0f) + 120.0f :
							rgb_max == B ? (((R - G) / rgb_range) * 60.0f) + 240.0f :
							0.0f);

		const float saturation = (rgb_max == 0.0f ? 0.0f : rgb_range / rgb_max);
		const float value = rgb_max;

		//in the new color, r = h, g = s, b = v, a = a
		return FLinearColor(hue, saturation, value, A);
	}

	/**
	* pow table for fast FColor -> FLinearColor conversion.
	*
	* FMath::Pow( i / 255.f, 2.2f )
	*/
	float FLinearColor::pow_22_one_over_255_table[256] =
	{
		0.0f, 5.07705190066176E-06f, 2.33280046660989E-05f, 5.69217657121931E-05f, 0.000107187362341244f, 0.000175123977503027f, 0.000261543754548491f, 0.000367136269815943f, 0.000492503787191433f,
		0.000638182842167022f, 0.000804658499513058f, 0.000992374304074325f, 0.0012017395224384f, 0.00143313458967186f, 0.00168691531678928f, 0.00196341621339647f, 0.00226295316070643f,
		0.00258582559623417f, 0.00293231832393836f, 0.00330270303200364f, 0.00369723957890013f, 0.00411617709328275f, 0.00455975492252602f, 0.00502820345685554f, 0.00552174485023966f,
		0.00604059365484981f, 0.00658495738258168f, 0.00715503700457303f, 0.00775102739766061f, 0.00837311774514858f, 0.00902149189801213f, 0.00969632870165823f, 0.0103978022925553f,
		0.0111260823683832f, 0.0118813344348137f, 0.0126637200315821f, 0.0134733969401426f, 0.0143105193748841f, 0.0151752381596252f, 0.0160677008908869f, 0.01698805208925f, 0.0179364333399502f,
		0.0189129834237215f, 0.0199178384387857f, 0.0209511319147811f, 0.0220129949193365f, 0.0231035561579214f, 0.0242229420675342f, 0.0253712769047346f, 0.0265486828284729f, 0.027755279978126f,
		0.0289911865471078f, 0.0302565188523887f, 0.0315513914002264f, 0.0328759169483838f, 0.034230206565082f, 0.0356143696849188f, 0.0370285141619602f, 0.0384727463201946f, 0.0399471710015256f,
		0.0414518916114625f, 0.0429870101626571f, 0.0445526273164214f, 0.0461488424223509f, 0.0477757535561706f, 0.049433457555908f, 0.0511220500564934f, 0.052841625522879f, 0.0545922772817603f,
		0.0563740975519798f, 0.0581871774736854f, 0.0600316071363132f, 0.0619074756054558f, 0.0638148709486772f, 0.0657538802603301f, 0.0677245896854243f, 0.0697270844425988f, 0.0717614488462391f,
		0.0738277663277846f, 0.0759261194562648f, 0.0780565899581019f, 0.080219258736215f, 0.0824142058884592f, 0.0846415107254295f, 0.0869012517876603f, 0.0891935068622478f, 0.0915183529989195f,
		0.0938758665255778f, 0.0962661230633397f, 0.0986891975410945f, 0.1011451642096f, 0.103634096655137f, 0.106156067812744f, 0.108711149979039f, 0.11129941482466f, 0.113920933406333f,
		0.116575776178572f, 0.119264013005047f, 0.121985713169619f, 0.124740945387051f, 0.127529777813422f, 0.130352278056244f, 0.1332085131843f, 0.136098549737202f, 0.139022453734703f,
		0.141980290685736f, 0.144972125597231f, 0.147998022982685f, 0.151058046870511f, 0.154152260812165f, 0.157280727890073f, 0.160443510725344f, 0.16364067148529f, 0.166872271890766f,
		0.170138373223312f, 0.173439036332135f, 0.176774321640903f, 0.18014428915439f, 0.183548998464951f, 0.186988508758844f, 0.190462878822409f, 0.193972167048093f, 0.19751643144034f,
		0.201095729621346f, 0.204710118836677f, 0.208359655960767f, 0.212044397502288f, 0.215764399609395f, 0.219519718074868f, 0.223310408341127f, 0.227136525505149f, 0.230998124323267f,
		0.23489525921588f, 0.238827984272048f, 0.242796353254002f, 0.24680041960155f, 0.2508402364364f, 0.254915856566385f, 0.259027332489606f, 0.263174716398492f, 0.267358060183772f,
		0.271577415438375f, 0.275832833461245f, 0.280124365261085f, 0.284452061560024f, 0.288815972797219f, 0.293216149132375f, 0.297652640449211f, 0.302125496358853f, 0.306634766203158f,
		0.311180499057984f, 0.315762743736397f, 0.32038154879181f, 0.325036962521076f, 0.329729032967515f, 0.334457807923889f, 0.339223334935327f, 0.344025661302187f, 0.348864834082879f,
		0.353740900096629f, 0.358653905926199f, 0.363603897920553f, 0.368590922197487f, 0.373615024646202f, 0.37867625092984f, 0.383774646487975f, 0.388910256539059f, 0.394083126082829f,
		0.399293299902674f, 0.404540822567962f, 0.409825738436323f, 0.415148091655907f, 0.420507926167587f, 0.425905285707146f, 0.43134021380741f, 0.436812753800359f, 0.442322948819202f,
		0.44787084180041f, 0.453456475485731f, 0.45907989242416f, 0.46474113497389f, 0.470440245304218f, 0.47617726539744f, 0.481952237050698f, 0.487765201877811f, 0.493616201311074f,
		0.49950527660303f, 0.505432468828216f, 0.511397818884879f, 0.517401367496673f, 0.523443155214325f, 0.529523222417277f, 0.535641609315311f, 0.541798355950137f, 0.547993502196972f,
		0.554227087766085f, 0.560499152204328f, 0.566809734896638f, 0.573158875067523f, 0.579546611782525f, 0.585972983949661f, 0.592438030320847f, 0.598941789493296f, 0.605484299910907f,
		0.612065599865624f, 0.61868572749878f, 0.625344720802427f, 0.632042617620641f, 0.638779455650817f, 0.645555272444934f, 0.652370105410821f, 0.659223991813387f, 0.666116968775851f,
		0.673049073280942f, 0.680020342172095f, 0.687030812154625f, 0.694080519796882f, 0.701169501531402f, 0.708297793656032f, 0.715465432335048f, 0.722672453600255f, 0.729918893352071f,
		0.737204787360605f, 0.744530171266715f, 0.751895080583051f, 0.759299550695091f, 0.766743616862161f, 0.774227314218442f, 0.781750677773962f, 0.789313742415586f, 0.796916542907978f,
		0.804559113894567f, 0.81224148989849f, 0.819963705323528f, 0.827725794455034f, 0.835527791460841f, 0.843369730392169f, 0.851251645184515f, 0.859173569658532f, 0.867135537520905f,
		0.875137582365205f, 0.883179737672745f, 0.891262036813419f, 0.899384513046529f, 0.907547199521614f, 0.915750129279253f, 0.923993335251873f, 0.932276850264543f, 0.940600707035753f,
		0.948964938178195f, 0.957369576199527f, 0.96581465350313f, 0.974300202388861f, 0.982826255053791f, 0.99139284359294f, 1.0f
	};

	/*
	 * table for fast FColor -> FLinear conversion
	 *
	 * Color > 0.04045 ? pow(Color * (1.0 / 1.055) + 0.0521327, 2.4) : Color * (1.0 / 12.92)
	 */
	float FLinearColor::srgb_to_linear_table[256] =
	{
		0.0f,
		0.000303526983548838f, 0.000607053967097675f, 0.000910580950646512f, 0.00121410793419535f, 0.00151763491774419f,
		0.00182116190129302f, 0.00212468888484186f, 0.0024282158683907f, 0.00273174285193954f, 0.00303526983548838f,
		0.00334653564113713f, 0.00367650719436314f, 0.00402471688178252f, 0.00439144189356217f, 0.00477695332960869f,
		0.005181516543916f, 0.00560539145834456f, 0.00604883284946662f, 0.00651209061157708f, 0.00699540999852809f,
		0.00749903184667767f, 0.00802319278093555f, 0.0085681254056307f, 0.00913405848170623f, 0.00972121709156193f,
		0.0103298227927056f, 0.0109600937612386f, 0.0116122449260844f, 0.012286488094766f, 0.0129830320714536f,
		0.0137020827679224f, 0.0144438433080002f, 0.0152085141260192f, 0.0159962930597398f, 0.0168073754381669f,
		0.0176419541646397f, 0.0185002197955389f, 0.0193823606149269f, 0.0202885627054049f, 0.0212190100154473f,
		0.0221738844234532f, 0.02315336579873f, 0.0241576320596103f, 0.0251868592288862f, 0.0262412214867272f,
		0.0273208912212394f, 0.0284260390768075f, 0.0295568340003534f, 0.0307134432856324f, 0.0318960326156814f,
		0.0331047661035236f, 0.0343398063312275f, 0.0356013143874111f, 0.0368894499032755f, 0.0382043710872463f,
		0.0395462347582974f, 0.0409151963780232f, 0.0423114100815264f, 0.0437350287071788f, 0.0451862038253117f,
		0.0466650857658898f, 0.0481718236452158f, 0.049706565391714f, 0.0512694577708345f, 0.0528606464091205f,
		0.0544802758174765f, 0.0561284894136735f, 0.0578054295441256f, 0.0595112375049707f, 0.0612460535624849f,
		0.0630100169728596f, 0.0648032660013696f, 0.0666259379409563f, 0.0684781691302512f, 0.070360094971063f,
		0.0722718499453493f, 0.0742135676316953f, 0.0761853807213167f, 0.0781874210336082f, 0.0802198195312533f,
		0.0822827063349132f, 0.0843762107375113f, 0.0865004612181274f, 0.0886555854555171f, 0.0908417103412699f,
		0.0930589619926197f, 0.0953074657649191f, 0.0975873462637915f, 0.0998987273569704f, 0.102241732185838f,
		0.104616483176675f, 0.107023102051626f, 0.109461709839399f, 0.1119324268857f, 0.114435372863418f,
		0.116970666782559f, 0.119538426999953f, 0.122138771228724f, 0.124771816547542f, 0.127437679409664f,
		0.130136475651761f, 0.132868320502552f, 0.135633328591233f, 0.138431613955729f, 0.141263290050755f,
		0.144128469755705f, 0.147027265382362f, 0.149959788682454f, 0.152926150855031f, 0.155926462553701f,
		0.158960833893705f, 0.162029374458845f, 0.16513219330827f, 0.168269398983119f, 0.171441099513036f,
		0.174647402422543f, 0.17788841473729f, 0.181164242990184f, 0.184474993227387f, 0.187820771014205f,
		0.191201681440861f, 0.194617829128147f, 0.198069318232982f, 0.201556252453853f, 0.205078735036156f,
		0.208636868777438f, 0.212230756032542f, 0.215860498718652f, 0.219526198320249f, 0.223227955893977f,
		0.226965872073417f, 0.23074004707378f, 0.23455058069651f, 0.238397572333811f, 0.242281120973093f,
		0.246201325201334f, 0.250158283209375f, 0.254152092796134f, 0.258182851372752f, 0.262250655966664f,
		0.266355603225604f, 0.270497789421545f, 0.274677310454565f, 0.278894261856656f, 0.283148738795466f,
		0.287440836077983f, 0.291770648154158f, 0.296138269120463f, 0.300543792723403f, 0.304987312362961f,
		0.309468921095997f, 0.313988711639584f, 0.3185467763743f, 0.323143207347467f, 0.32777809627633f,
		0.332451534551205f, 0.337163613238559f, 0.341914423084057f, 0.346704054515559f, 0.351532597646068f,
		0.356400142276637f, 0.361306777899234f, 0.36625259369956f, 0.371237678559833f, 0.376262121061519f,
		0.381326009488037f, 0.386429431827418f, 0.39157247577492f, 0.396755228735618f, 0.401977777826949f,
		0.407240209881218f, 0.41254261144808f, 0.417885068796976f, 0.423267667919539f, 0.428690494531971f,
		0.434153634077377f, 0.439657171728079f, 0.445201192387887f, 0.450785780694349f, 0.456411021020965f,
		0.462076997479369f, 0.467783793921492f, 0.473531493941681f, 0.479320180878805f, 0.485149937818323f,
		0.491020847594331f, 0.496932992791578f, 0.502886455747457f, 0.50888131855397f, 0.514917663059676f,
		0.520995570871595f, 0.527115123357109f, 0.533276401645826f, 0.539479486631421f, 0.545724458973463f,
		0.552011399099209f, 0.558340387205378f, 0.56471150325991f, 0.571124827003694f, 0.577580437952282f,
		0.584078415397575f, 0.590618838409497f, 0.597201785837643f, 0.603827336312907f, 0.610495568249093f,
		0.617206559844509f, 0.623960389083534f, 0.630757133738175f, 0.637596871369601f, 0.644479679329661f,
		0.651405634762384f, 0.658374814605461f, 0.665387295591707f, 0.672443154250516f, 0.679542466909286f,
		0.686685309694841f, 0.693871758534824f, 0.701101889159085f, 0.708375777101046f, 0.71569349769906f,
		0.723055126097739f, 0.730460737249286f, 0.737910405914797f, 0.745404206665559f, 0.752942213884326f,
		0.760524501766589f, 0.768151144321824f, 0.775822215374732f, 0.783537788566466f, 0.791297937355839f,
		0.799102735020525f, 0.806952254658248f, 0.81484656918795f, 0.822785751350956f, 0.830769873712124f,
		0.838799008660978f, 0.846873228412837f, 0.854992605009927f, 0.863157210322481f, 0.871367116049835f,
		0.879622393721502f, 0.887923114698241f, 0.896269350173118f, 0.904661171172551f, 0.913098648557343f,
		0.921581853023715f, 0.930110855104312f, 0.938685725169219f, 0.947306533426946f, 0.955973349925421f,
		0.964686244552961f, 0.973445287039244f, 0.982250546956257f, 0.991102093719252f, 1.0f
	};
}
