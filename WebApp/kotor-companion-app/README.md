# KOTOR.ai Companion Web App

A Next.js companion web application for KOTOR.ai that provides campaign tracking, party chat, and LLM story insights.

## Features

### 🎮 Campaign Tracking
- **Real-time Progress**: Live campaign progress tracking
- **Quest Journal**: Interactive quest log with completion status
- **Character Sheets**: Detailed character information and stats
- **Timeline View**: Visual campaign timeline with major events
- **Statistics Dashboard**: Comprehensive campaign analytics

### 💬 Party Chat
- **Real-time Messaging**: Live chat between party members
- **Voice Messages**: Audio message support
- **File Sharing**: Share screenshots and campaign files
- **Emoji Reactions**: React to messages with custom emojis
- **Thread Discussions**: Organize conversations by topic

### 🤖 LLM Story Insights
- **Story Analysis**: AI-powered analysis of campaign events
- **Character Insights**: Deep dive into character development
- **Narrative Predictions**: AI predictions for story outcomes
- **Moral Choice Analysis**: Impact analysis of player decisions
- **Relationship Tracking**: Monitor companion relationships

### 📱 Cross-Platform
- **Responsive Design**: Works on desktop, tablet, and mobile
- **PWA Support**: Install as a Progressive Web App
- **Offline Mode**: Basic functionality works offline
- **Push Notifications**: Real-time notifications for campaign events

## Tech Stack

- **Framework**: Next.js 14 with App Router
- **Language**: TypeScript
- **Styling**: Tailwind CSS + Shadcn/ui
- **Database**: Supabase (PostgreSQL)
- **Real-time**: Supabase Realtime
- **Authentication**: Supabase Auth
- **State Management**: Zustand
- **API**: tRPC for type-safe APIs
- **LLM Integration**: OpenAI GPT-4 / Anthropic Claude
- **Deployment**: Vercel

## Project Structure

```
kotor-companion-app/
├── src/
│   ├── app/                    # Next.js App Router
│   │   ├── (auth)/            # Authentication pages
│   │   ├── dashboard/         # Main dashboard
│   │   ├── campaign/          # Campaign pages
│   │   ├── chat/              # Party chat
│   │   ├── insights/          # LLM insights
│   │   └── api/               # API routes
│   ├── components/            # React components
│   │   ├── ui/                # Shadcn/ui components
│   │   ├── campaign/          # Campaign-specific components
│   │   ├── chat/              # Chat components
│   │   └── insights/          # Insights components
│   ├── lib/                   # Utility libraries
│   │   ├── supabase/          # Supabase client
│   │   ├── trpc/              # tRPC setup
│   │   ├── llm/               # LLM integration
│   │   └── utils/             # Helper functions
│   ├── hooks/                 # Custom React hooks
│   ├── stores/                # Zustand stores
│   └── types/                 # TypeScript types
├── public/                    # Static assets
├── docs/                      # Documentation
└── tests/                     # Test files
```

## Key Components

### Campaign Dashboard
```typescript
// src/components/campaign/CampaignDashboard.tsx
interface CampaignDashboard {
  campaignId: string;
  playerData: PlayerData;
  questLog: Quest[];
  timeline: TimelineEvent[];
  statistics: CampaignStats;
}
```

### Party Chat
```typescript
// src/components/chat/PartyChat.tsx
interface PartyChat {
  sessionId: string;
  participants: Player[];
  messages: Message[];
  onSendMessage: (message: string) => void;
  onSendVoice: (audio: Blob) => void;
}
```

### LLM Insights
```typescript
// src/components/insights/StoryInsights.tsx
interface StoryInsights {
  campaignData: CampaignData;
  insights: LLMInsight[];
  predictions: StoryPrediction[];
  characterAnalysis: CharacterAnalysis[];
}
```

## Database Schema

### Tables
```sql
-- Campaigns
CREATE TABLE campaigns (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  name TEXT NOT NULL,
  description TEXT,
  creator_id UUID REFERENCES auth.users(id),
  created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  is_active BOOLEAN DEFAULT true,
  settings JSONB DEFAULT '{}'
);

-- Campaign Sessions
CREATE TABLE campaign_sessions (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  campaign_id UUID REFERENCES campaigns(id),
  session_name TEXT NOT NULL,
  start_time TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  end_time TIMESTAMP WITH TIME ZONE,
  participants JSONB DEFAULT '[]',
  session_data JSONB DEFAULT '{}'
);

-- Players
CREATE TABLE players (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  user_id UUID REFERENCES auth.users(id),
  campaign_id UUID REFERENCES campaigns(id),
  character_name TEXT NOT NULL,
  character_data JSONB DEFAULT '{}',
  joined_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  is_active BOOLEAN DEFAULT true
);

-- Messages
CREATE TABLE messages (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  session_id UUID REFERENCES campaign_sessions(id),
  sender_id UUID REFERENCES players(id),
  message_type TEXT DEFAULT 'text', -- 'text', 'voice', 'file', 'system'
  content TEXT,
  metadata JSONB DEFAULT '{}',
  created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Quest Log
CREATE TABLE quest_log (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  campaign_id UUID REFERENCES campaigns(id),
  quest_id TEXT NOT NULL,
  quest_name TEXT NOT NULL,
  quest_data JSONB DEFAULT '{}',
  status TEXT DEFAULT 'active', -- 'active', 'completed', 'failed'
  created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  updated_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);

-- Timeline Events
CREATE TABLE timeline_events (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  campaign_id UUID REFERENCES campaigns(id),
  event_type TEXT NOT NULL,
  event_title TEXT NOT NULL,
  event_description TEXT,
  event_data JSONB DEFAULT '{}',
  timestamp TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
  importance_level INTEGER DEFAULT 1
);

-- LLM Insights
CREATE TABLE llm_insights (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  campaign_id UUID REFERENCES campaigns(id),
  insight_type TEXT NOT NULL, -- 'story', 'character', 'prediction', 'analysis'
  title TEXT NOT NULL,
  content TEXT NOT NULL,
  confidence_score FLOAT DEFAULT 0.0,
  metadata JSONB DEFAULT '{}',
  created_at TIMESTAMP WITH TIME ZONE DEFAULT NOW()
);
```

## API Endpoints

### tRPC Routers
```typescript
// src/lib/trpc/routers/campaign.ts
export const campaignRouter = router({
  getCampaign: publicProcedure
    .input(z.object({ id: z.string() }))
    .query(async ({ input }) => {
      // Get campaign data
    }),
  
  updateProgress: protectedProcedure
    .input(z.object({ 
      campaignId: z.string(),
      progressData: z.object({})
    }))
    .mutation(async ({ input }) => {
      // Update campaign progress
    }),
});

// src/lib/trpc/routers/chat.ts
export const chatRouter = router({
  getMessages: protectedProcedure
    .input(z.object({ sessionId: z.string() }))
    .query(async ({ input }) => {
      // Get chat messages
    }),
  
  sendMessage: protectedProcedure
    .input(z.object({
      sessionId: z.string(),
      content: z.string(),
      type: z.enum(['text', 'voice', 'file'])
    }))
    .mutation(async ({ input }) => {
      // Send message
    }),
});

// src/lib/trpc/routers/insights.ts
export const insightsRouter = router({
  generateInsights: protectedProcedure
    .input(z.object({ campaignId: z.string() }))
    .mutation(async ({ input }) => {
      // Generate LLM insights
    }),
  
  getInsights: protectedProcedure
    .input(z.object({ campaignId: z.string() }))
    .query(async ({ input }) => {
      // Get existing insights
    }),
});
```

## Real-time Features

### Supabase Realtime
```typescript
// src/hooks/useRealtimeCampaign.ts
export function useRealtimeCampaign(campaignId: string) {
  const [campaignData, setCampaignData] = useState<CampaignData | null>(null);
  
  useEffect(() => {
    const channel = supabase
      .channel(`campaign:${campaignId}`)
      .on('postgres_changes', {
        event: '*',
        schema: 'public',
        table: 'campaigns',
        filter: `id=eq.${campaignId}`
      }, (payload) => {
        // Handle campaign updates
      })
      .subscribe();
    
    return () => {
      supabase.removeChannel(channel);
    };
  }, [campaignId]);
  
  return { campaignData };
}
```

## LLM Integration

### Story Analysis
```typescript
// src/lib/llm/storyAnalysis.ts
export async function generateStoryInsights(campaignData: CampaignData): Promise<LLMInsight[]> {
  const prompt = `
    Analyze this KOTOR campaign data and provide insights:
    
    Campaign: ${campaignData.name}
    Timeline: ${JSON.stringify(campaignData.timeline)}
    Characters: ${JSON.stringify(campaignData.characters)}
    Choices: ${JSON.stringify(campaignData.moralChoices)}
    
    Provide insights on:
    1. Story progression and pacing
    2. Character development arcs
    3. Moral choice consequences
    4. Relationship dynamics
    5. Potential future story directions
  `;
  
  const response = await openai.chat.completions.create({
    model: "gpt-4",
    messages: [{ role: "user", content: prompt }],
    temperature: 0.7,
  });
  
  return parseInsights(response.choices[0].message.content);
}
```

## Installation & Setup

```bash
# Clone the repository
git clone https://github.com/your-org/kotor-companion-app.git
cd kotor-companion-app

# Install dependencies
npm install

# Set up environment variables
cp .env.example .env.local
# Edit .env.local with your Supabase and OpenAI credentials

# Run database migrations
npm run db:migrate

# Start development server
npm run dev
```

## Environment Variables

```bash
# .env.local
NEXT_PUBLIC_SUPABASE_URL=your_supabase_url
NEXT_PUBLIC_SUPABASE_ANON_KEY=your_supabase_anon_key
SUPABASE_SERVICE_ROLE_KEY=your_service_role_key

OPENAI_API_KEY=your_openai_api_key
ANTHROPIC_API_KEY=your_anthropic_api_key

NEXTAUTH_SECRET=your_nextauth_secret
NEXTAUTH_URL=http://localhost:3000
```

## Deployment

### Vercel Deployment
```bash
# Install Vercel CLI
npm i -g vercel

# Deploy to Vercel
vercel --prod
```

### Docker Deployment
```dockerfile
# Dockerfile
FROM node:18-alpine AS deps
WORKDIR /app
COPY package*.json ./
RUN npm ci --only=production

FROM node:18-alpine AS builder
WORKDIR /app
COPY . .
COPY --from=deps /app/node_modules ./node_modules
RUN npm run build

FROM node:18-alpine AS runner
WORKDIR /app
ENV NODE_ENV production
COPY --from=builder /app/public ./public
COPY --from=builder /app/.next/standalone ./
COPY --from=builder /app/.next/static ./.next/static

EXPOSE 3000
ENV PORT 3000

CMD ["node", "server.js"]
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## License

MIT License - see LICENSE file for details
